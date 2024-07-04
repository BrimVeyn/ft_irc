/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 17:27:52 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 17:27:55 by albeninc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleTopicCommand(int clientSocket, std::istringstream & lineStream) {
	std::string channel, topic;
	lineStream >> channel;
	std::getline(lineStream, topic);
	// Get the iterator to the vector of channels for the given client
	std::vector<std::string>& channels = userInfo_[clientSocket].channels;

	// Use std::find to search for the channel
	std::vector<std::string>::iterator it = std::find(channels.begin(), channels.end(), channel);

	if (channelInfo_[channel].isTopicProtected && !isOperator("@" + userInfo_[clientSocket].nickname, channel)) {
		std::string serverResponse = getServerReply(ERR_CHANOPRIVSNEEDED, clientSocket);
		serverResponse += " " + channel + " :Not enough privilege\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return ;
	}

    if (it != channels.end()) {
        channelInfo_[channel].topic = topic;
        std::string response = getCommandPrefix(clientSocket) + "TOPIC " + channel + topic + "\r\n";
		printResponse(SERVER, response);
		send(clientSocket, response.c_str(), response.size(), 0);
        broadcastMessage(clientSocket, response, channel);
	} else {
		std::string serverResponse = getServerReply(ERR_NOSUCHCHANNEL, clientSocket);
		serverResponse += " " + channel + " :No such channel\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return ;
	}
}
