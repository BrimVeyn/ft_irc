/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:50:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/18 16:20:49 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleJoinCommand(int clientSocket, std::istringstream & lineStream) {
	std::string channel;
	lineStream >> channel;

    userInfo_[clientSocket].channels.push_back(channel);
    std::string joinMessage = getCommandPrefix(clientSocket) + "JOIN " + channel + "\r\n";
	send(clientSocket, joinMessage.c_str(), joinMessage.size(), 0);
    broadcastMessage(clientSocket, joinMessage, channel);
}
