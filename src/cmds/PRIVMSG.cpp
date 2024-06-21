/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:50:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 11:09:04 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <iterator>
#include <sstream>

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
