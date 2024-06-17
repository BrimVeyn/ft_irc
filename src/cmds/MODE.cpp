/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:52:12 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/17 16:56:23 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleModeCommand(int clientSocket, std::istringstream & lineStream) {
	std::string channel, mode;
	lineStream >> channel >> mode;
    // Mode handling implementation goes here
    std::string response = ":" + nicknames_[clientSocket] + " MODE " + channel + " " + mode + "\r\n";
    broadcastMessage(clientSocket, response, channel);
}
