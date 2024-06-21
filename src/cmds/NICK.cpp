/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 13:22:17 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 14:04:06 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <map>
#include <vector>

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
		if (std::isalnum(nickname[i]) == false && nickname[i] != '-'){
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

void IRCServer::sendToOtherChannels(int clientSocket, std::string response) {
	std::vector<std::string> userChannels = userInfo_[clientSocket].channels;
	std::vector<std::string>::iterator it = userChannels.begin();
	std::vector<std::string> ppl_seen;

	for (; it != userChannels.end(); it++) {
		std::vector<std::string> cMember = channelInfo_[*it].members;
		std::vector<std::string> cOperator = channelInfo_[*it].operators;
		std::vector<std::string>::iterator mit = cMember.begin();
		std::vector<std::string>::iterator oit = cOperator.begin();

		for (; mit != cMember.end(); mit++) {
			if (std::find(ppl_seen.begin(), ppl_seen.end(), *mit) == ppl_seen.end() && *mit != userInfo_[clientSocket].nickname) {
				printResponse(SERVER, response);
				std::cout << "DEBUG MIT = " <<  *mit << getClientSocket(*mit) << std::endl;
				send(getClientSocket(*mit), response.c_str(), response.size(), 0);
				ppl_seen.push_back(*mit);
			}
		}
		for (; oit != cOperator.end(); oit++) {
			std::string real_username = (*oit).substr(1);
			if (std::find(ppl_seen.begin(), ppl_seen.end(), real_username) == ppl_seen.end() && real_username != userInfo_[clientSocket].nickname) {
				printResponse(SERVER, response);
				std::cout << "DEBUG OIT = " <<  real_username << getClientSocket(real_username) << std::endl;
				send(getClientSocket(real_username), response.c_str(), response.size(), 0);
				ppl_seen.push_back(real_username);
			}
		}
	}
}

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

	//Broadcast nick change to all channel the users's in
	sendToOtherChannels(clientSocket, response);
	//-----------------------------------------//
}
