#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

#include <string>

class ClientHandler {
public:
    ClientHandler(int socket);
    void processMessage(const std::string& message);
private:
    int socket_;
};

#endif // CLIENTHANDLER_HPP
