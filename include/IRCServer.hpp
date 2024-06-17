#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <string>

class IRCServer {
public:
    IRCServer(int port, const std::string& password);
    void start();
private:
    int port_;
    std::string password_;
    void handleClient(int clientSocket);
};

#endif // IRCSERVER_HPP
