#include "../include/IRCServer.hpp"
#include <cstdlib>
#include <iostream>
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
    while (true) {
        struct pollfd fds[1];
        fds[0].fd = serverSocket_;
        fds[0].events = POLLIN;

        int pollCount = poll(fds, 1, -1);
        if (pollCount == -1) {
            std::cerr << "Poll failed: " << strerror(errno) << std::endl;
            close(serverSocket_);
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN) {
            sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, &clientAddrLen);
            if (clientSocket == -1) {
                std::cerr << "Failed to accept client: " << strerror(errno) << std::endl;
                continue;
            }

            clients_.push_back(clientSocket);
            std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
            handleClient(clientSocket);
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
    }

    std::cout << "Received message: " << buffer << std::endl;

    const char* response = "Welcome to the IRC server!\n";
    if (send(clientSocket, response, std::strlen(response), 0) == -1) {
        std::cerr << "Failed to send response: " << strerror(errno) << std::endl;
        closeSocket(clientSocket);
    }
}

void IRCServer::closeSocket(int socket) {
    close(socket);
    clients_.erase(std::remove(clients_.begin(), clients_.end(), socket), clients_.end());
}
