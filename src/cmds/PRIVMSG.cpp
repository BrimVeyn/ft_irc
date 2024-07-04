/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 15:30:56 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 16:08:47 by albeninc         ###   ########.fr       */
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

void IRCServer::handlePrivmsgCommand(int clientSocket, std::istringstream &lineStream) {
    std::string target;
    lineStream >> target;
    std::string privmsg;
    getline(lineStream, privmsg); // Extract the remaining message, which should start with ':' indicating the start of the actual message

    // Format the message properly with the command prefix
    std::string formattedMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + target + " " + privmsg + "\r\n";

    if (target[0] == '#') {
        // Channel message
        if (!isValidChannel(target)) {
            std::string noSuchChannel = getServerReply(ERR_NOSUCHCHANNEL, clientSocket);
            noSuchChannel += " " + target + " :No such channel on ft_irc\r\n";
            printResponse(SERVER, noSuchChannel);
            send(clientSocket, noSuchChannel.c_str(), noSuchChannel.size(), 0);
            return;
        }
        broadcastMessage(clientSocket, formattedMessage, target);
    } else {
        if (target == "GameBot") {
			std::cout << "Yeah" << std::endl;
            handleGameCommand(clientSocket, privmsg);
        } else {
            int targetSocket = getClientSocket(target);
            if (targetSocket == -1 || send(targetSocket, formattedMessage.c_str(), formattedMessage.size(), 0) == -1) {
                std::string serverResponse = getServerReply(ERR_NOSUCHNICK, clientSocket);
                serverResponse += " " + target + " :No such nick in ft_irc\r\n";
                printResponse(SERVER, serverResponse);
                send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
            }
        }
    }
    printResponse(SERVER, formattedMessage);
}
