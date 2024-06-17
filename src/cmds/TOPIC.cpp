/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:51:55 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/17 16:56:01 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleTopicCommand(int clientSocket, std::istringstream & lineStream) {
	std::string channel, topic;
	lineStream >> channel;
	std::getline(lineStream, topic);
    if (channels_[clientSocket] == channel) {
        topics_[channel] = topic;
        std::string response = ":" + nicknames_[clientSocket] + " TOPIC " + channel + " :" + topic + "\r\n";
        broadcastMessage(clientSocket, response, channel);
    }
}
