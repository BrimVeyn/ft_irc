#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <string>
#include <vector>
#include <map>

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

    void initializeSocket();
    void acceptConnections();
    void handleClient(int clientSocket);
    void closeSocket(int socket);
};

#endif // IRCSERVER_HPP
