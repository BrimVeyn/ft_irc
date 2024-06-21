/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 11:03:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 11:08:33 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

std::string addNumberToStr(const std::string & nickname, int nb) {
	std::stringstream ss;

	if (nb != 0)
		ss << nickname << nb;
	else
		ss << nickname;

	std::string str;
	ss >> str;

	return str;
}

// Gestion de la commande NICK
void IRCServer::handleNickCommand(int clientSocket, std::istringstream & lineStream) {
	std::string nickname;
	lineStream >> nickname;

	// ----- Ajout suffix si duplication de nickname ----- //
	int suffix = 0;
	for (std::vector<int>::iterator it = clients_.begin(); it != clients_.end(); it++) {
		if (*it == clientSocket) {
			continue;
		}
		if (userInfo_[*it].nickname == addNumberToStr(nickname, suffix)) {
			suffix += 1;
        }
	}
	if (suffix != 0)
		nickname = addNumberToStr(nickname, suffix);
	//-----------------------------------------//

	// ----- Gestion premiere connection ----- //
	if (userInfo_[clientSocket].is_register == false){
		userInfo_[clientSocket].nickname = nickname;
		return;
	}
	//-----------------------------------------//

    // ----- Change nickname dans les channels ----- //
    std::map<std::string, channelInfo>::iterator it = channelInfo_.begin();
    for (;it != channelInfo_.end(); it++){
        std::vector<std::string>::iterator m_it = it->second.members.begin();
        std::vector<std::string>::iterator o_it = it->second.operators.begin();
        for (; m_it != it->second.members.end(); m_it++){
            // std::cout << YELLOW << "[DEBUG] comparing members: " << *m_it << " " << userInfo_[clientSocket].nickname << RESET_COLOR << std::endl;
            if (*m_it == userInfo_[clientSocket].nickname){
                *m_it = nickname;
                break;
            }
        }
        for (; o_it != it->second.operators.end(); o_it++){
            // std::cout << YELLOW << "[DEBUG] comparing operators: " << *o_it << " " << '@' + userInfo_[clientSocket].nickname << RESET_COLOR << std::endl;
            if (*o_it == '@' + userInfo_[clientSocket].nickname){
                *o_it = nickname;
                break;
            }
        }
    }
    //-----------------------------------------//
	
	std::string response = getCommandPrefix(clientSocket) + "NICK :" + nickname + "\r\n";
	userInfo_[clientSocket].nickname = nickname;

	printResponse(SERVER, response);
    
    // Send the NICK response to the client
    if (send(clientSocket, response.c_str(), response.size(), 0) == -1) {
        std::cerr << "Failed to send NICK response" << std::endl;
    }
	//-----------------------------------------//
}
