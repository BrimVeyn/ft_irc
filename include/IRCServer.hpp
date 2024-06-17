#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <string>
#include <vector>
#include <map>
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

class IRCServer {
public:
    IRCServer(int port, const std::string& password);
    ~IRCServer();
    void start();

private:
	typedef void (IRCServer::*CommandHandler)(int, std::istringstream&);

    int port_;
    std::string password_;
    int serverSocket_;
    std::vector<int> clients_;
    std::vector<struct pollfd> poll_fds_;
    std::map<int, std::string> nicknames_; // Map pour stocker les pseudonymes des clients
    std::map<int, std::string> usernames_; // Map pour stocker les noms d'utilisateurs des clients
    std::map<int, std::string> channels_;  // Map pour stocker les canaux des clients
    std::map<std::string, std::string> topics_;  // Map pour stocker les sujets des channels
	std::map<std::string, CommandHandler> commandMap_;

    void initializeSocket();
    void acceptConnections();
    void handleClient(int clientSocket);
    void closeSocket(int socket);
    void broadcastMessage(int senderSocket, const std::string& message, const std::string& channel);
	void handleCmds(std::string message, int clientSocket);

    void handleNickCommand(int clientSocket, std::istringstream& lineStream);
    void handleUserCommand(int clientSocket, std::istringstream& lineStream);
    void handleJoinCommand(int clientSocket, std::istringstream& lineStream);
    void handlePartCommand(int clientSocket, std::istringstream& lineStream);
    void handlePrivmsgCommand(int clientSocket, std::istringstream& lineStream);
    void handleKickCommand(int clientSocket, std::istringstream& lineStream);
    void handleInviteCommand(int clientSocket, std::istringstream& lineStream);
    void handleTopicCommand(int clientSocket, std::istringstream& lineStream);
    void handleModeCommand(int clientSocket, std::istringstream& lineStream);
    void handlePassCommand(int clientSocket, std::istringstream& lineStream);
    void handleCapCommand(int clientSocket, std::istringstream& lineStream);
};

#endif // IRCSERVER_HPP
