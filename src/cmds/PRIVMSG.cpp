/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 12:05:59 by nbardavi          #+#    #+#             */
/*   Updated: 2024/06/21 16:27:01 by nbardavi         ###   ########.fr       */
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

void IRCServer::handlePrivmsgCommand(int clientSocket, std::istringstream & lineStream) {

    std::string target;
    lineStream >> target;
    std::string privmsg;
    std::getline(lineStream, privmsg);
	

    std::string formattedMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + target + privmsg + "\r\n" ;
    if (target[0] == '#'){
		//------------- Filter --------------//
		if (isValidChannel(target) == false){
		std::string noSuchChannel = getServerReply(ERR_NOSUCHCHANNEL, clientSocket);
		noSuchChannel += " " + target + " :No such channel on ft_irc\r\n";
		printResponse(SERVER, noSuchChannel);
		send(clientSocket, noSuchChannel.c_str(), noSuchChannel.size(), 0);
		return ;
		}
		//----------------------------------//

        broadcastMessage(clientSocket, formattedMessage, target);
        return;
    }
	
	//------------- Sending msg and error handling --------------//
    if (send(getClientSocket(target), formattedMessage.c_str(), formattedMessage.size(), 0) == -1){
		std::string serverResponse = getServerReply(ERR_NOSUCHNICK, clientSocket);
		serverResponse += " " + target + " :No such nick in ft_irc\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return;
	}
	//------------- Filter --------------//
	printResponse(SERVER, formattedMessage);
}
