/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:51:33 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 16:08:45 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleInviteCommand(int clientSocket, std::istringstream & lineStream) {
	std::string user, channel;
	lineStream >> user >> channel;
	
	if (isValidNickname(user) == false){
		std::string serverResponse = getServerReply(ERR_NOSUCHNICK, clientSocket);
		serverResponse += " " + user + " :Invalid username\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return ;
	}

	if ((isOperator("@" + userInfo_[clientSocket].nickname, channel) && channelInfo_[channel].isInviteOnly) || !channelInfo_[channel].isInviteOnly) {

		std::string serverResponse = getCommandPrefix(clientSocket);
		serverResponse += "INVITE " + user + " " + channel + "\r\n";

		if (send(getClientSocket(user), serverResponse.c_str(), serverResponse.size(), 0) == -1) {
			//nosuchuser;
			std::string serverResponse = getServerReply(ERR_NOSUCHNICK, clientSocket);
			serverResponse += " " + user + " :No such user\r\n";
			printResponse(SERVER, serverResponse);
			send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
			return ;
		} else {
			channelInfo_[channel].inviteList.push_back(user);
			std::string serverResponse = getServerReply(RPL_INVITING, clientSocket);
			serverResponse += " " + user + " " + channel + "\r\n";
			printResponse(SERVER, serverResponse);
			send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
			return ;
		}
	}
}
