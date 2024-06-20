/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:50:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/20 17:11:58 by bvan-pae         ###   ########.fr       */
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
	std::cout << RED << "target: " << target << std::endl << "msg" << privmsg << std::endl;
    // std::string formattedMessage = ":" + userInfo_[clientSocket].nickname + " PRIVMSG " + target + " :" + privmsg + "\r\n";
	std::cout << RED << formattedMessage << std::endl << RESET_COLOR;
	send(getClientSocket(target), formattedMessage.c_str(), formattedMessage.size(), 0);
	std::cout << RED << getClientSocket(target) << std::endl << RESET_COLOR;
}
