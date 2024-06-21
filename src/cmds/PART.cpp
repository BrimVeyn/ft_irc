/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PART.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:52:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 10:18:51 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::removeMember(int clientSocket, std::string channel) {
	std::vector<std::string> & memberVect = channelInfo_[channel].members;
	std::vector<std::string> & operatorVect = channelInfo_[channel].operators;
	std::vector<std::string>::iterator it;

	if ((it = std::find(memberVect.begin(),memberVect.end(), userInfo_[clientSocket].nickname)) != memberVect.end()) {
		memberVect.erase(it);
	} else if ((it = std::find(operatorVect.begin(),operatorVect.end(), "@" + userInfo_[clientSocket].nickname)) != operatorVect.end()) {
		operatorVect.erase(it);
	}
}

void IRCServer::handlePartCommand(int clientSocket, std::istringstream & lineStream) {

	std::string channel;
	lineStream >> channel;

	// Get the iterator to the vector of channels for the given client
	std::vector<std::string>& channels = userInfo_[clientSocket].channels;
	// Use std::find to search for the channel
	std::vector<std::string>::iterator it = std::find(channels.begin(), channels.end(), channel);

    if (it != channels.end()) {
        std::string response = ":" + userInfo_[clientSocket].nickname + " PART " + channel + "\r\n";
		std::string user_message = getCommandPrefix(clientSocket) + "PART " + channel + " .\r\n";
		printResponse(SERVER, response);
		send(clientSocket, user_message.c_str(), user_message.size(), 0);
        broadcastMessage(clientSocket, user_message, channel);
		channels.erase(it);
		removeMember(clientSocket, channel);
		// userInfo_[clientSocket].channels.erase()
		channelInfo_[channel].userCount -= 1;
    }
	//remove user from db
}
