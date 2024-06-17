#include "../include/IRCServer.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>

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
    : port_(port), password_(password), serverSocket_(-1) {}

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

    // Split the message into lines
    std::istringstream iss(message);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty()) {
            continue;
        }
        // Remove any trailing '\r'
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }
        std::cout << "Processing line: " << line << std::endl;

        // Parse the command
        std::istringstream lineStream(line);
        std::string command;
        lineStream >> command;

        std::cout << "Command received: " << command << std::endl;

        if (command == "NICK") {
            std::string nickname;
            lineStream >> nickname;
            handleNickCommand(clientSocket, nickname);
        } else if (command == "USER") {
            std::string userInfo;
            std::getline(lineStream, userInfo);
            handleUserCommand(clientSocket, userInfo);
        } else if (command == "JOIN") {
            std::string channel;
            lineStream >> channel;
            handleJoinCommand(clientSocket, channel);
        } else if (command == "CAP") {
            std::string subcommand;
            lineStream >> subcommand;
            if (subcommand == "LS" || subcommand == "LIST") {
                std::string response = "CAP * LS :\r\n";
                send(clientSocket, response.c_str(), response.size(), 0);
            } else if (subcommand == "END") {
                std::cout << "CAP negotiation ended for client " << clientSocket << std::endl;
            }
        } else {
            // Broadcast the message to all other clients
            broadcastMessage(clientSocket, line);
        }
    }
}

void IRCServer::broadcastMessage(int senderSocket, const std::string& message) {
    for (size_t i = 0; i < clients_.size(); ++i) {
        int clientSocket = clients_[i];
        if (clientSocket != senderSocket) {
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


// Gestion de la commande USER
void IRCServer::handleUserCommand(int clientSocket, const std::string& userInfo) {
    usernames_[clientSocket] = userInfo;
    std::string response = ":localhost 001 " + nicknames_[clientSocket] + " :Welcome to the IRC server\r\n";
    std::cout << "Handling USER command for client " << clientSocket << " with user info " << userInfo << std::endl;
    send(clientSocket, response.c_str(), response.size(), 0);
}

// Gestion de la commande NICK
void IRCServer::handleNickCommand(int clientSocket, const std::string& nickname) {
    nicknames_[clientSocket] = nickname;
    std::string response = ":" + nickname + " NICK " + nickname + "\r\n";
    std::cout << "Handling NICK command for client " << clientSocket << " with nickname " << nickname << std::endl;
    send(clientSocket, response.c_str(), response.size(), 0);
}

void IRCServer::handleJoinCommand(int clientSocket, const std::string& channel) {
    channels_[clientSocket] = channel;
    std::string response = ":" + nicknames_[clientSocket] + " JOIN " + channel + "\r\n";
    std::cout << "Handling JOIN command for client " << clientSocket << " for channel " << channel << std::endl;
    send(clientSocket, response.c_str(), response.size(), 0);
    std::string joinMessage = ":" + nicknames_[clientSocket] + "!" + usernames_[clientSocket] + "@localhost JOIN " + channel + "\r\n";
    broadcastMessage(clientSocket, joinMessage);
}
