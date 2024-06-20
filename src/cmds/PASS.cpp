/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PASS.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:58:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/20 09:24:37 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handlePassCommand(int clientSocket, std::istringstream & lineStream) {
	std::string receivedPassword;
	lineStream >> receivedPassword;
	if (receivedPassword == password_) {
		// std::string welcomeMessage = ":Welcome to the Internet Relay Network " + userInfo_[clientSocket].nickname + "!" + userInfo_[clientSocket].username + "@" + userInfo_[clientSocket].server_addr + "\r\n";
		// std::cout << YELLOW << "[DEBUG] welcomeMessage: " << welcomeMessage << std::endl;
		// send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
	} else {
		std::string errorMessage = "Authentication failed!\n";
		send(clientSocket, errorMessage.c_str(), errorMessage.size(), 0);
		close(clientSocket);
	}
}
