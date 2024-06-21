/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 12:05:59 by nbardavi          #+#    #+#             */
/*   Updated: 2024/06/21 15:49:50 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <iterator>
#include <sstream>

//if target is user --> send user
//if target is channel --> broadcast channel

int IRCServer::getClientSocket(const std::string &nickname){
	std::map<int, userInfo>::iterator it = userInfo_.begin();
	for (; it != userInfo_.end(); it++){
		if (it->second.nickname == nickname)
			return it->first;
	}
	return -1;
}

static bool nickFilter(const std::string & nickname){
	for (int i = 0; nickname[i]; i++){
		if (std::isalnum(nickname[i]) == false && nickname[i] != '-'){
			return false;
		}
	}
	return true;
}

void IRCServer::handlePrivmsgCommand(int clientSocket, std::istringstream & lineStream) {

    std::string target;
    lineStream >> target;
    std::string privmsg;
    std::getline(lineStream, privmsg);
	
	if (nickFilter(target) == false){
		std::string invalidNickname = getServerReply(ERR_ERRONEUSNICKNAME, clientSocket);
		invalidNickname += " " + target + " :Erroneus nickname\r\n";
		printResponse(SERVER, invalidNickname);
		send(clientSocket, invalidNickname.c_str(), invalidNickname.size(), 0);
		return ;
	}

    std::string formattedMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + target + privmsg + "\r\n" ;
    if (target[0] == '#'){
        broadcastMessage(clientSocket, formattedMessage, target);
        return;
    }
    printResponse(SERVER, formattedMessage);
    send(getClientSocket(target), formattedMessage.c_str(), formattedMessage.size(), 0);
}
