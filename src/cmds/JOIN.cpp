/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:50:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/19 17:02:42 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <iterator>
#include <sstream>

std::string IRCServer::getServerReply(int numeric, int clientSocket) {
	std::stringstream numeric_stream;
	numeric_stream << numeric;

	std::string numeric_string = numeric_stream.str();

	std::string response = ":" + userInfo_[clientSocket].server_addr;
	response += " " + numeric_string +  " " + userInfo_[clientSocket].nickname;

	return response;
}

std::string IRCServer::getMemberList(const std::string channel) {
	std::stringstream members_stream;
	std::vector<std::string> memberVect = channelInfo_[channel].members;
	std::vector<std::string> operatorVect = channelInfo_[channel].operators;

	for (std::vector<std::string>::iterator it = operatorVect.begin(); it != operatorVect.end(); it++) {
		members_stream << *it;
		//Initialize an iterator to the next member
		std::vector<std::string>::iterator it_p1 = it;
		std::advance(it_p1, 1);

		//If there is a next member, add spacing
		if (it_p1 != memberVect.end() || !memberVect.size())
			members_stream << " ";
	}

	for (std::vector<std::string>::iterator it = memberVect.begin(); it != memberVect.end(); it++) {
		members_stream << *it;
		//Initialize an iterator to the next member
		std::vector<std::string>::iterator it_p1 = it;
		std::advance(it_p1, 1);

		//If there is a next member, add spacing
		if (it_p1 != memberVect.end())
			members_stream << " ";
	}
	return members_stream.str();
}

void IRCServer::setOperator(std::string nickname, std::string channel, int clientSocket) {
	// std::string serverResponse = getServerReply(RPL_CHANNELMODEIS, clientSocket);
	// serverResponse += " " + channel + "\r\n";
	// std::cout << SERVER << RED << serverResponse << RESET_COLOR << std::endl;
	// send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
	(void) nickname;
	(void) channel;
	(void) clientSocket;
}

void IRCServer::sendChannelMemberList(int clientSocket, std::string channel) {

	channelInfo_[channel].operators.push_back("@" + userInfo_[clientSocket].nickname);

	std::string memberList = getMemberList(channel);
	std::string nameReply = getServerReply(RPL_NAMREPLY, clientSocket) + " = ";
	nameReply += channel + " :" + memberList + "\r\n";
	printResponse(SERVER, nameReply);
	send(clientSocket, nameReply.c_str(), nameReply.size(), 0);

	//send RPL_ENDOFNAMES :localhost 366 bvan-pae #lol :End of /NAMES list.
	std::string endOfNames = getServerReply(RPL_ENDOFNAMES, clientSocket) + " ";
	endOfNames += channel + " :End of member list\r\n";
	send(clientSocket, endOfNames.c_str(), endOfNames.size(), 0);
}

void IRCServer::sendChannelTopic(int clientSocket, std::string channel) {
	std::string topic = channelInfo_[channel].topic;

	if (topic.size()) {
		std::string hasTopicResponse = getServerReply(RPL_TOPIC ,clientSocket);
		hasTopicResponse += " " + channel + topic + "\r\n";
		printResponse(SERVER, hasTopicResponse);
		send(clientSocket, hasTopicResponse.c_str(), hasTopicResponse.size(), 0);
    }  else  {
		std::string noTopicResponse = getServerReply(RPL_NOTOPIC ,clientSocket);
		noTopicResponse += " " + channel + " :No topic set\r\n";
		printResponse(SERVER, noTopicResponse);
		send(clientSocket, noTopicResponse.c_str(), noTopicResponse.size(), 0);
	}
}

void IRCServer::handleJoinCommand(int clientSocket, std::istringstream & lineStream) {
	std::string channel;
	lineStream >> channel;

    userInfo_[clientSocket].channels.push_back(channel);

	if (!getMemberList(channel).size())
		setOperator(userInfo_[clientSocket].nickname, channel, clientSocket);
	else  {
		channelInfo_[channel].members.push_back(userInfo_[clientSocket].nickname);
	}

	///--------JOIN message -----//
    std::string joinMessage = getCommandPrefix(clientSocket) + "JOIN " + channel + "\r\n";

	send(clientSocket, joinMessage.c_str(), joinMessage.size(), 0);
    broadcastMessage(clientSocket, joinMessage, channel);
	//-------------------------//

	//display member list;
	sendChannelMemberList(clientSocket, channel);

	//display topic;
	sendChannelTopic(clientSocket, channel);
}
