#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>

class IRCServer {
public:
    IRCServer(int port, const std::string& password);
    ~IRCServer();
    void start();

private:
    int port_;
    std::string password_;
    int serverSocket_;
    std::vector<int> clients_;
    std::vector<struct pollfd> poll_fds_;
    std::map<int, std::string> nicknames_; // Map pour stocker les pseudonymes des clients
    std::map<int, std::string> usernames_; // Map pour stocker les noms d'utilisateurs des clients
    std::map<int, std::string> channels_;  // Map pour stocker les canaux des clients
    std::map<std::string, std::string> topics_;  // Map pour stocker les sujets des channels

    void initializeSocket();
    void acceptConnections();
    void handleClient(int clientSocket);
    void closeSocket(int socket);
    void broadcastMessage(int senderSocket, const std::string& message, const std::string& channel);

    void handleNickCommand(int clientSocket, const std::string& nickname);
    void handleUserCommand(int clientSocket, const std::string& userInfo);
    void handleJoinCommand(int clientSocket, const std::string& channel);
    void handlePartCommand(int clientSocket, const std::string& channel);
    void handlePrivmsgCommand(int clientSocket, const std::string& target, const std::string& message);
    void handleKickCommand(int clientSocket, const std::string& channel, const std::string& user);
    void handleInviteCommand(int clientSocket, const std::string& channel, const std::string& user);
    void handleTopicCommand(int clientSocket, const std::string& channel, const std::string& topic);
    void handleModeCommand(int clientSocket, const std::string& channel, const std::string& mode);
};

#endif // IRCSERVER_HPP
