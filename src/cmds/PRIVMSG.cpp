/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:50:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/17 17:02:05 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handlePrivmsgCommand(int clientSocket, std::istringstream & lineStream) {

	std::string target;
	lineStream >> target;
	std::string privmsg;
	std::getline(lineStream, privmsg);

    std::string channel = channels_[clientSocket];
    std::string formattedMessage = ":" + nicknames_[clientSocket] + " PRIVMSG " + target + " :" + privmsg + "\r\n";
    broadcastMessage(clientSocket, formattedMessage, channel);
}
