#include "../include/IRCServer.hpp"
#include <cstring>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <sys/socket.h>

// Classe de comparaison pour remove_if
class FdComparator {
public:
    FdComparator(int socket) : socket_(socket) {}
    bool operator()(const struct pollfd& pfd) {
        return pfd.fd == socket_;
    }
private:
    int socket_;
};

IRCServer::IRCServer(int port, const std::string& password)
    : port_(port), password_(password), serverSocket_(-1) {

	//Initialize commandMap, which points to every command we handle
    commandMap_["NICK"] = &IRCServer::handleNickCommand;
    commandMap_["USER"] = &IRCServer::handleUserCommand;
    commandMap_["JOIN"] = &IRCServer::handleJoinCommand;
    commandMap_["PART"] = &IRCServer::handlePartCommand;
    commandMap_["PRIVMSG"] = &IRCServer::handlePrivmsgCommand;
    commandMap_["KICK"] = &IRCServer::handleKickCommand;
    commandMap_["INVITE"] = &IRCServer::handleInviteCommand;
    commandMap_["TOPIC"] = &IRCServer::handleTopicCommand;
    commandMap_["MODE"] = &IRCServer::handleModeCommand;
    commandMap_["CAP"] = &IRCServer::handleCapCommand;
    commandMap_["PASS"] = &IRCServer::handlePassCommand;

	//Set available channel modes
	availableModes_ = "+i-i+k-k+o-o+t-t+l-l";
}

IRCServer::~IRCServer() {
    if (serverSocket_ != -1) {
        close(serverSocket_);
    }
    for (size_t i = 0; i < clients_.size(); ++i) {
        close(clients_[i]);
    }
	for (std::map<std::string, channelInfo>::iterator it = channelInfo_.begin(); it != channelInfo_.end(); it++) {
		channelInfo_.erase(it);
	}
}

void IRCServer::initializeSocket() {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == -1) {
        std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Failed to set socket options: " << strerror(errno) << std::endl;
        close(serverSocket_);
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind socket: " << strerror(errno) << std::endl;
        close(serverSocket_);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket_, 10) == -1) {
        std::cerr << "Failed to listen on socket: " << strerror(errno) << std::endl;
        close(serverSocket_);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << port_ << std::endl;
}

void IRCServer::start() {
    initializeSocket();
    acceptConnections();
}

void IRCServer::acceptConnections() {
    struct pollfd server_fd;
    server_fd.fd = serverSocket_;
    server_fd.events = POLLIN;
    poll_fds_.push_back(server_fd);

    while (true) {
        int pollCount = poll(poll_fds_.data(), poll_fds_.size(), -1);
        if (pollCount == -1) {
            std::cerr << "Poll failed: " << strerror(errno) << std::endl;
            close(serverSocket_);
            exit(EXIT_FAILURE);
        }

        for (size_t i = 0; i < poll_fds_.size(); ++i) {
            if (poll_fds_[i].revents & POLLIN) {
                if (poll_fds_[i].fd == serverSocket_) {
                    sockaddr_in clientAddr;
                    socklen_t clientAddrLen = sizeof(clientAddr);
                    int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, &clientAddrLen);
                    if (clientSocket == -1) {
                        std::cerr << "Failed to accept client: " << strerror(errno) << std::endl;
                        continue;
                    }
					
                    struct pollfd client_fd;
                    client_fd.fd = clientSocket;
                    client_fd.events = POLLIN;
                    poll_fds_.push_back(client_fd);

                    clients_.push_back(clientSocket);
					userInfo_[clientSocket].is_register = false;
                    std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
                } else {
                    handleClient(poll_fds_[i].fd);
                }
            }
        }
    }
}

void IRCServer::broadcastMessage(int senderSocket, const std::string& message, const std::string& channel) {
	printResponse(BROADCAST, message);
    for (size_t i = 0; i < clients_.size(); ++i) {
        int clientSocket = clients_[i];
		// Get the iterator to the vector of channels for the given client
		std::vector<std::string>& channels = userInfo_[clientSocket].channels;

		// Use std::find to search for the channel
		std::vector<std::string>::iterator it = std::find(channels.begin(), channels.end(), channel);
        if (clientSocket != senderSocket && it != channels.end()) {
            if (send(clientSocket, message.c_str(), message.size(), 0) == -1) {
                closeSocket(clientSocket);
            }
        }
    }
}

void IRCServer::closeSocket(int client_socket) {
    close(client_socket);
    poll_fds_.erase(std::remove_if(poll_fds_.begin(), poll_fds_.end(), FdComparator(client_socket)), poll_fds_.end());
    clients_.erase(std::remove(clients_.begin(), clients_.end(), client_socket), clients_.end());
	userInfo_.erase(client_socket);
    std::cout << "Client disconnected: " << client_socket << std::endl;
}

void IRCServer::handleCmds(std::string message, int clientSocket) {

    // Split the message into lines
    std::istringstream iss(message);
	std::istringstream pass_check(message);
    std::string line;

	//-------------------Password check-----------------//
	std::string last_line;
	bool has_pass = false;

	for (int i = 0;std::getline(pass_check, last_line); i++) {
		if (i == 0 && std::strncmp("CAP LS", last_line.c_str(), 6)) {
			has_pass = true;
			break;
		}
		if (!std::strncmp("PASS", last_line.c_str(), 4)) {
			has_pass = true;
		}
	}
	if (!has_pass) {
		std::string errorMessage = "Authentication failed!\n";
		send(clientSocket, errorMessage.c_str(), errorMessage.size(), 0);
		close(clientSocket);
	}
	//------------------------------------------------//

	while (std::getline(iss, line)) {
		if (line.empty())
			continue;
		// Remove any trailing '\r'
		if (!line.empty() && line[line.size() - 1] == '\r') {
			line.erase(line.size() - 1);
		}
		printResponse(CLIENT, line);

		// Parse the command
		std::istringstream lineStream(line);
		std::string command;
		lineStream >> command;

		std::map<std::string, CommandHandler>::iterator it = commandMap_.find(command);
		if (it != commandMap_.end()) {
			CommandHandler handler = it->second;
			(this->*handler)(clientSocket, lineStream);
		} else {
			// std::cout << RED << "commande:" << command << std::endl << RESET_COLOR;
			std::string channel = command;
			broadcastMessage(clientSocket, line, channel);
		}
	}
}

void IRCServer::handleClient(int clientSocket) {
    char buffer[512];
    std::memset(buffer, 0, sizeof(buffer));

	usleep(200);
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == -1) {
        std::cerr << "Failed to receive data: " << strerror(errno) << std::endl;
        closeSocket(clientSocket);
        return;
    } else if (bytesReceived == 0) {
        // Client disconnected
        std::cerr << "Client disconnected: " << clientSocket << std::endl;
        closeSocket(clientSocket);
        return;
    }


    std::string message(buffer, bytesReceived);
    // std::cout << "Received message from client " << clientSocket << ": " << message << std::endl;

	handleCmds(message, clientSocket);
}

