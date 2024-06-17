/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:50:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/17 16:53:47 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleJoinCommand(int clientSocket, std::istringstream & lineStream) {
	std::string channel;
	lineStream >> channel;

    channels_[clientSocket] = channel;
    std::string response = ":" + nicknames_[clientSocket] + " JOIN " + channel + "\r\n";
    std::cout << "Handling JOIN command for client " << clientSocket << " for channel " << channel << std::endl;
    send(clientSocket, response.c_str(), response.size(), 0);
    std::string joinMessage = ":" + nicknames_[clientSocket] + "!" + usernames_[clientSocket] + "@localhost JOIN " + channel + "\r\n";
    broadcastMessage(clientSocket, joinMessage, channel);
}
