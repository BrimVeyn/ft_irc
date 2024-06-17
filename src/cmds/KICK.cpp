/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:51:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/17 16:55:13 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleKickCommand(int clientSocket, std::istringstream & lineStream) {

	std::string channel, user;
	lineStream >> channel >> user;

    if (channels_[clientSocket] == channel) {
        // Rechercher le client avec le pseudonyme donné et le retirer du canal
        for (std::map<int, std::string>::iterator it = channels_.begin(); it != channels_.end(); ++it) {
            if (it->second == channel && nicknames_[it->first] == user) {
                std::string response = ":" + nicknames_[clientSocket] + " KICK " + channel + " " + user + " :Kicked by operator\r\n";
                send(it->first, response.c_str(), response.size(), 0);
                channels_.erase(it);
                return;
            }
        }
    }
}
