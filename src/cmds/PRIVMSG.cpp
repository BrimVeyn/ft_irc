/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 12:05:59 by nbardavi          #+#    #+#             */
/*   Updated: 2024/06/21 12:06:00 by nbardavi         ###   ########.fr       */
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
// :NICKNAME!USER@HOST PRIVMSG <target> :<message>

void IRCServer::handlePrivmsgCommand(int clientSocket, std::istringstream & lineStream) {

    std::string target;
    lineStream >> target;
    std::string privmsg;
    std::getline(lineStream, privmsg);
    std::string formattedMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + target + privmsg + "\r\n" ;
    if (target[0] == '#'){
        printResponse(BROADCAST, formattedMessage);
        broadcastMessage(clientSocket, formattedMessage, target);
        return;
    }
    printResponse(SERVER, formattedMessage);
    send(getClientSocket(target), formattedMessage.c_str(), formattedMessage.size(), 0);
}
