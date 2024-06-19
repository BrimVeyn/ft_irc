/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:51:55 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/19 11:20:57 by bvan-pae         ###   ########.fr       */
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

    if (it != channels.end()) {
        channelInfo_[channel].topic = topic;
        std::string response = getCommandPrefix(clientSocket) + "TOPIC " + channel + topic + "\r\n";
		std::cout << SERVER << RED << response << RESET_COLOR << std::endl;
		send(clientSocket, response.c_str(), response.size(), 0);
        broadcastMessage(clientSocket, response, channel);
    } else {
		(void) clientSocket;
		//error wrong channel
	}
}
