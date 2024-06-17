/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:51:33 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/17 16:55:35 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleInviteCommand(int clientSocket, std::istringstream & lineStream) {
	std::string channel, user;
	lineStream >> channel >> user;
    if (channels_[clientSocket] == channel) {
        // Rechercher le client avec le pseudonyme donné et l'inviter
        for (std::map<int, std::string>::iterator it = nicknames_.begin(); it != nicknames_.end(); ++it) {
            if (it->second == user) {
                std::string response = ":" + nicknames_[clientSocket] + " INVITE " + user + " " + channel + "\r\n";
                send(it->first, response.c_str(), response.size(), 0);
                return;
            }
        }
    }
}
