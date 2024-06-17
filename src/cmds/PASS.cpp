/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PASS.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:58:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/17 17:00:57 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handlePassCommand(int clientSocket, std::istringstream & lineStream) {
	std::string receivedPassword;
	lineStream >> receivedPassword;
	if (receivedPassword == password_) {
		std::string welcomeMessage = "Welcome to the IRC server!\n";
		send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
	} else {
		std::string errorMessage = "Authentication failed!\n";
		send(clientSocket, errorMessage.c_str(), errorMessage.size(), 0);
		close(clientSocket);
	}
}
