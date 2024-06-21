/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 12:05:47 by nbardavi          #+#    #+#             */
/*   Updated: 2024/06/21 12:16:36 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <map>

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

bool filterNick(const std::string & nickname){
	for (int i = 0; nickname[i]; i++){
		if (isalnum(nickname[i]) == false){
			return false;
		}
	}
	return true;
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

struct ReplaceNick {
	const std::string& oldNick;
	const std::string& newNick;

	ReplaceNick(const std::string& oldNick, const std::string& newNick) : oldNick(oldNick), newNick(newNick) {}

	void operator()(std::pair<const std::string, channelInfo>& channel) const {
		std::vector<std::string>& members = channel.second.members;
		std::vector<std::string>& operators = channel.second.operators;
		std::replace(members.begin(), members.end(), oldNick, newNick);
		std::replace(operators.begin(), operators.end(), "@" + oldNick, "@" + newNick);
	}
};

// Gestion de la commande NICK
void IRCServer::handleNickCommand(int clientSocket, std::istringstream & lineStream) {
	std::string nickname;
	lineStream >> nickname;

	//:server_name 432 * nickname :Erroneous nickname
	if (filterNick(nickname) == false) {
		std::string message = ":ft_irc 432 * " + nickname + ":Erroneous nickname\r\n";
		printResponse(SERVER, message);
		send(clientSocket, message.c_str(), message.size(), 0);
		return;
	}
	// ----- Ajout suffix si duplication de nickname ----- //
	handleNickCollision(clientSocket, nickname);
	//-----------------------------------------//

	// ----- Gestion premiere connection ----- //
	if (userInfo_[clientSocket].is_authenticated == false){
		userInfo_[clientSocket].nickname = nickname;
		return;
	}
	//-----------------------------------------//

    // ----- Change nickname dans les channels ----- //
	std::string oldNick = userInfo_[clientSocket].nickname;
    std::for_each(channelInfo_.begin(), channelInfo_.end(), ReplaceNick(oldNick, nickname));
    //-----------------------------------------//
	
	std::string response = getCommandPrefix(clientSocket) + "NICK :" + nickname + "\r\n";
	userInfo_[clientSocket].nickname = nickname;
	//Send nick Response
	printResponse(SERVER, response);
    send(clientSocket, response.c_str(), response.size(), 0);
	//-----------------------------------------//
}
