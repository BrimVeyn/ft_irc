/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:51:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 10:19:04 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleKickCommand(int clientSocket, std::istringstream & lineStream) {

	std::string channel, user, reason;
	lineStream >> channel >> user;

	reason = lineStream.str();
	reason = reason.substr(reason.find(":"));
	
	if (isOperator("@" + userInfo_[clientSocket].nickname, channel)) {
		std::string serverResponse = getCommandPrefix(clientSocket);
		serverResponse += "KICK " + channel + " " + user + " ";
		if (reason.size() == 1)
			serverResponse += DEFAULT_KICK_REASON;
		else
			serverResponse += reason;
		serverResponse += "\r\n";
		printResponse(SERVER, serverResponse);
		if (send(getClientSocket(user), serverResponse.c_str(), serverResponse.size(), 0) == -1) {
			//nosuchuser;
			std::string serverResponse = getServerReply(ERR_NOSUCHNICK, clientSocket);
			serverResponse += " " + user + " :No such user\r\n";
			printResponse(SERVER, serverResponse);
			send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
			return ;
		} else {
			broadcastMessage(getClientSocket(user), serverResponse, channel);
			removeMember(getClientSocket(user), channel);
			channelInfo_[channel].userCount -= 1;
			//userinfo
		}
	} else {
		std::string serverResponse = getServerReply(ERR_CHANOPRIVSNEEDED, clientSocket);
		serverResponse += " " + channel + " :Not enough privilege\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
	}
}
