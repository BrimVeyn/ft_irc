#include "../../include/IRCServer.hpp"

// Gestion de la commande NICK
void IRCServer::handleNickCommand(int clientSocket, std::istringstream & lineStream) {
	std::string nickname;
	lineStream >> nickname;
    // Store the nickname in the map
    nicknames_[clientSocket] = nickname;

    // Create the response message for the NICK command
    std::string response = ":" + nickname + " NICK " + nickname + "\r\n";
    
    // Create a user-friendly message indicating the nickname change
    std::string userMessage = "Your nickname has been changed to " + nickname + "\r\n";

    std::cout << "Handling NICK command for client " << clientSocket << " with nickname " << nickname << std::endl;

    // Send the NICK response to the client
    if (send(clientSocket, response.c_str(), response.size(), 0) == -1) {
        std::cerr << "Failed to send NICK response" << std::endl;
    } else {
        std::cerr << "Successfully sent NICK response" << std::endl;
    }

    // Send the user-friendly message to the client
    if (send(clientSocket, userMessage.c_str(), userMessage.size(), 0) == -1) {
        std::cerr << "Failed to send user-friendly message" << std::endl;
    } else {
        std::cerr << "Successfully sent user-friendly message" << std::endl;
    }
}
