/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PART.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 17:27:31 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 17:27:33 by albeninc         ###   ########.fr       */
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
	std::vector<std::string> & userChannels = userInfo_[clientSocket].channels;
	it = userChannels.begin();
	if ((it = std::find(userChannels.begin(), userChannels.end(), channel)) != userChannels.end()){
		userChannels.erase(it);
		channelInfo_[channel].userCount -= 1;
	}
}

void IRCServer::handlePartCommand(int clientSocket, std::istringstream & lineStream) {

	std::string channel;
	lineStream >> channel;
	
	if (isValidChannel(channel) == false) {
		std::string noSuchChannel = getServerReply(ERR_NOSUCHCHANNEL, clientSocket);
		noSuchChannel += " " + channel + " :No such channel on ft_irc\r\n";
		printResponse(SERVER, noSuchChannel);
		send(clientSocket, noSuchChannel.c_str(), noSuchChannel.size(), 0);
		return ;
	}

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
    } else {
		std::string hasTopicResponse = getServerReply(ERR_USERNOTINCHANNEL,clientSocket);
		hasTopicResponse += " " + userInfo_[clientSocket].nickname + " " + channel + " :You're not in this channel\r\n";
		printResponse(SERVER, hasTopicResponse);
		send(clientSocket, hasTopicResponse.c_str(), hasTopicResponse.size(), 0);
	}
}
