/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 10:04:40 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 10:53:54 by bvan-pae         ###   ########.fr       */
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

void IRCServer::handleNickCollision(int clientSocket, std::string & nickname) {

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
}

// Gestion de la commande NICK
void IRCServer::handleNickCommand(int clientSocket, std::istringstream & lineStream) {
	std::string nickname;
	lineStream >> nickname;

	// ----- Ajout suffix si duplication de nickname ----- //
	handleNickCollision(clientSocket, nickname);
	//-----------------------------------------//

	// ----- Gestion premiere connection ----- //
	if (userInfo_[clientSocket].is_authenticated == false){
		userInfo_[clientSocket].nickname = nickname;
		return;
	}
	//-----------------------------------------//

	std::string response = getCommandPrefix(clientSocket) + "NICK :" + nickname + "\r\n";
	userInfo_[clientSocket].nickname = nickname;
	//Send nick Response
	printResponse(SERVER, response);
    send(clientSocket, response.c_str(), response.size(), 0);
	//-----------------------------------------//
}
