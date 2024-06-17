#include "../include/IRCServer.hpp"

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
}

IRCServer::~IRCServer() {
    if (serverSocket_ != -1) {
        close(serverSocket_);
    }
    for (size_t i = 0; i < clients_.size(); ++i) {
        close(clients_[i]);
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
                    std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
                } else {
                    handleClient(poll_fds_[i].fd);
                }
            }
        }
    }
}

void IRCServer::broadcastMessage(int senderSocket, const std::string& message, const std::string& channel) {
    std::cout << "Broadcasting message from client " << senderSocket << " in channel " << channel << ": " << message << std::endl;
    for (size_t i = 0; i < clients_.size(); ++i) {
        int clientSocket = clients_[i];
        if (clientSocket != senderSocket && channels_[clientSocket] == channel) {
            std::cout << "Sending message to client " << clientSocket << " in channel " << channel << std::endl;
            if (send(clientSocket, message.c_str(), message.size(), 0) == -1) {
                std::cerr << "Failed to send message to client " << clientSocket << ": " << strerror(errno) << std::endl;
                closeSocket(clientSocket);
            }
        }
    }
}

void IRCServer::closeSocket(int socket) {
    close(socket);
    poll_fds_.erase(std::remove_if(poll_fds_.begin(), poll_fds_.end(), FdComparator(socket)), poll_fds_.end());
    clients_.erase(std::remove(clients_.begin(), clients_.end(), socket), clients_.end());
    nicknames_.erase(socket); // Remove nickname
    usernames_.erase(socket); // Remove username
    channels_.erase(socket);  // Remove channel
    std::cout << "Client disconnected: " << socket << std::endl;
}

void IRCServer::handleCmds(std::string message, int clientSocket) {

    // Split the message into lines
    std::istringstream iss(message);
    std::string line;

	while (std::getline(iss, line)) {
		if (line.empty())
			continue;
		// Remove any trailing '\r'
		if (!line.empty() && line[line.size() - 1] == '\r') {
			line.erase(line.size() - 1);
		}
		std::cout << "Processing line: " << line << std::endl;

		// Parse the command
		std::istringstream lineStream(line);
		std::string command;
		lineStream >> command;

		std::map<std::string, CommandHandler>::iterator it = commandMap_.find(command);
		if (it != commandMap_.end()) {
			CommandHandler handler = it->second;
			(this->*handler)(clientSocket, lineStream);
		} else {
			std::string channel = channels_[clientSocket];
			broadcastMessage(clientSocket, line, channel);
		}
	}
}

void IRCServer::handleClient(int clientSocket) {
    char buffer[512];
    std::memset(buffer, 0, sizeof(buffer));

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
    std::cout << "Received message from client " << clientSocket << ": " << message << std::endl;

	handleCmds(message, clientSocket);
    
}

