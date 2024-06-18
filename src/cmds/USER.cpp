/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:50:13 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/18 15:40:10 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

// Gestion de la commande USER
void IRCServer::handleUserCommand(int clientSocket, std::istringstream & lineStream) {
    std::string username, hostname, servername, realname;

    // Read the parameters from the userInfo string
    lineStream >> username >> hostname >> servername;

    // The rest of the line is the realname
    std::getline(lineStream, realname);

    // Store username in your data structure
	userInfo_[clientSocket].server_addr = servername;
    userInfo_[clientSocket].username = username;

	std::cout << MAGENTA << "username is " << username << RESET_COLOR << std::endl;
    // Example response (you might want to customize this)
    std::string response = ":localhost 001 " + userInfo_[clientSocket].nickname + " :Welcome to the IRC server\r\n";
    std::cout << "Handling USER command for client " << clientSocket << " with username " << username << std::endl;

    // Send response to the client
    send(clientSocket, response.c_str(), response.size(), 0);
}
