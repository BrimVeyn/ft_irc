/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PART.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:52:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/17 16:54:04 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handlePartCommand(int clientSocket, std::istringstream & lineStream) {

	std::string channel;
	lineStream >> channel;

    if (channels_[clientSocket] == channel) {
        std::string response = ":" + nicknames_[clientSocket] + " PART " + channel + "\r\n";
        std::cout << "Handling PART command for client " << clientSocket << " for channel " << channel << std::endl;
        broadcastMessage(clientSocket, response, channel);
        channels_.erase(clientSocket);
    }
}
