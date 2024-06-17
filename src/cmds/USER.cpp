/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:50:13 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/17 16:53:23 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

// Gestion de la commande USER
void IRCServer::handleUserCommand(int clientSocket, std::istringstream & lineStream) {
	std::string userInfo;
	std::getline(lineStream, userInfo);

    usernames_[clientSocket] = userInfo;
    std::string response = ":localhost 001 " + nicknames_[clientSocket] + " :Welcome to the IRC server\r\n";
    std::cout << "Handling USER command for client " << clientSocket << " with user info " << userInfo << std::endl;
    send(clientSocket, response.c_str(), response.size(), 0);
}
