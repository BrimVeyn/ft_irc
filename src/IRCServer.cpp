/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 17:28:06 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 17:28:07 by albeninc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCServer.hpp"
#include <cstddef>
#include <cstring>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include <ctime>
#include <stdio.h>

IRCServer* IRCServer::instance_ = NULL;

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

void IRCServer::cleanup(){
	if (instance_){
		delete instance_;
	}
}

std::string getCurrentDateTime() {
    std::time_t now = std::time(0);
    std::tm* now_tm = std::localtime(&now);

    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", now_tm);

    return std::string(buffer);
}

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
    commandMap_["PASS"] = &IRCServer::handlePassCommand;
	//Set available channel modes
	
	bancommands_ = "PING-PONG-WHOIS";
	availableModes_ = "+i-i+k-k+o-o+t-t+l-l";
	creationDate_ = getCurrentDateTime();

	instance_ = this;
    std::atexit(IRCServer::cleanup);
}

IRCServer::~IRCServer() {
	std::cout << std::endl << RED << "Server shutting down..." << RESET_COLOR << std::endl;
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

    std::cout << GREEN << "Server is listening on port " << port_ << RESET_COLOR << std::endl;
}

void IRCServer::handleSignal(int signal_num){
	exit (signal_num);
}

void IRCServer::initializeSignals(){

    std::memset(&sa, 0, sizeof(sa));

    // Set the signal handler function
    sa.sa_handler = IRCServer::handleSignal;

    // Set the flag to restart functions if interrupted by handler
    sa.sa_flags = SA_RESTART;

    // Set up the SIGINT handler
    if (sigaction(SIGINT, &sa, NULL) != 0) {
        std::cerr << "Error setting up signal handler: " << std::strerror(errno) << std::endl;
    }
}

void IRCServer::start() {
	initializeSignals();
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
					userInfo_[clientSocket].is_authenticated = false;
                    std::cout << SERVER_HEADER << YELLOW << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << RESET_COLOR << std::endl;
                } else {
                    handleClient(poll_fds_[i].fd);
                }
            }
        }
    }
}

void IRCServer::broadcastMessage(int senderSocket, const std::string& message, const std::string& channel) {
    for (size_t i = 0; i < clients_.size(); ++i) {
        int clientSocket = clients_[i];
		// Get the iterator to the vector of channels for the given client
		std::vector<std::string>& channels = userInfo_[clientSocket].channels;
		// std::vector<std::string>& channelss = userInfo_[senderSocket].channels;

		// Use std::find to search for the channel
		// std::cout << clientSocket << " | " << senderSocket << std::endl;
		// for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); it++) {
		// 	std::cout << "CC " << *it << std::endl;
		// }
		// for (std::vector<std::string>::iterator it = channelss.begin(); it != channelss.end(); it++) {
		// 	std::cout << "CS " << *it << std::endl;
		// }

		std::vector<std::string>::iterator it = std::find(channels.begin(), channels.end(), channel);
        if (clientSocket != senderSocket && it != channels.end()) {
			printResponse(BROADCAST, "to client " + userInfo_[clientSocket].nickname + " " + message);
            if (send(clientSocket, message.c_str(), message.size(), 0) == -1) {
                closeSocket(clientSocket);
            }
        }
    }
}

bool IRCServer::isValidNickname(const std::string& nickname) {
    if (nickname.length() < 1 || nickname.length() > 9) {
        return false;
    }

    if (!std::isalpha(nickname[0])) {
        return false;
    }

    std::string validChars = "-[]\\{}^_|";

    for (std::size_t i = 1; i < nickname.length(); ++i) {
        char c = nickname[i];
        if (!std::isalnum(c) && validChars.find(c) == std::string::npos) {
            return false;
        }
    }

    return true;
}

bool IRCServer::isValidChannel(const std::string& channel) {
    if (channel[0] != '#') {
        return false;
    }
    std::string validChars = "-[]\\{}^_|";

    for (std::size_t i = 1; i < channel.length(); ++i) {
        char c = channel[i];
        if (!std::isalnum(c) && validChars.find(c) == std::string::npos) {
            return false;
        }
    }
    return true;
}

void IRCServer::closeSocket(int client_socket) {
    std::vector<std::string> channels = userInfo_[client_socket].channels;
    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
        removeMember(client_socket, *it);
    }
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
		if (it != commandMap_.end() && bancommands_.find(command) == bancommands_.npos) {
			CommandHandler handler = it->second;
			(this->*handler)(clientSocket, lineStream);
		} else {
			//handle command not found
			std::string notFoundMsg = ":ft_irc 421 " + userInfo_[clientSocket].nickname + " " + command + " :Unknown command" + "\r\n";
			send(clientSocket, notFoundMsg.c_str(), notFoundMsg.size(), 0);
			printResponse(SERVER, notFoundMsg);
		}
	}
}

void setNonBlocking(int socket) {
    if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }
}

void IRCServer::handleClient(int clientSocket) {
    setNonBlocking(clientSocket);

    char buffer[1024];
    static std::string inputBuffer;
    ssize_t bytesReceived;

	bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

	if (bytesReceived > 0) {
		buffer[bytesReceived] = '\0';
		inputBuffer.append(buffer, bytesReceived);

		// Process each complete command in the inputBuffer
		size_t pos;
		// std::cout << "BUFFER = " << inputBuffer << std::endl;
		while ((pos = inputBuffer.find('\n')) != std::string::npos) {
			std::string command = inputBuffer.substr(0, pos);
			inputBuffer.erase(0, pos + 1);

			// Handle the command
			handleCmds(command, clientSocket);
		}
	} else if (bytesReceived == 0) {
		// Client closed the connection
		closeSocket(clientSocket);
	} else {
		// Handle recv errors
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			std::cerr << "Failed to receive data: " << strerror(errno) << std::endl;
			closeSocket(clientSocket);
		}
	}
}
