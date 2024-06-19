/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:52:12 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/19 17:30:04 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::printResponse(int mode, std::string message) {
	if (mode == CLIENT) {
		std::cout << CLIENT_HEADER << MAGENTA;
	} else if (mode == SERVER) {
		message = message.substr(0, message.size() - 2);
		std::cout << SERVER_HEADER << RED;
	} else if (mode == BROADCAST) {
		message = message.substr(0, message.size() - 2);
		std::cout << BROADCAST_HEADER << GREEN;
	}
	std::cout << message << RESET_COLOR << std::endl;
}

bool IRCServer::isUser(const std::string & str) {
	std::map<int, userInfo>::iterator it = userInfo_.begin();
	for (; it != userInfo_.end(); it++){
		if (it->second.nickname == str)
			return true;
	}
	return false;
}

int	IRCServer::getClientSocketFromUsername(const std::string &username) {
	std::map<int, userInfo>::iterator it = userInfo_.begin();
	for (; it != userInfo_.end(); it++){
		if (it->second.username == username)
			return it->first;
	}
	return -1;
}

void IRCServer::updateUserMode(int clientSocket, std::string user, std::string mode) {
	std::string modeResponse = getCommandPrefix(clientSocket);
	modeResponse += "MODE " + user + " " + mode + "\r\n";
	printResponse(SERVER, modeResponse);
	send(clientSocket, modeResponse.c_str(), modeResponse.size(), 0);
}

bool IRCServer::isOperator(std::string nickname, std::string channel) {
	std::vector<std::string> operators = channelInfo_[channel].operators;
	if (std::find(operators.begin(), operators.end(), nickname) != operators.end())
		return true;
	return false;
}

bool IRCServer::isModeValid(std::string mode) {
	if (std::find(availableModes_.begin(), availableModes_.end(), mode) != availableModes_.end())
		return true;
	return false;
}

void IRCServer::updateChannelMode(int clientSocket, std::string channel, std::string mode, std::string mode_option) {
	if (!mode.size() && !mode_option.size()) {

		std::string serverResponse = getServerReply(RPL_CHANNELMODEIS, clientSocket);
		serverResponse += " " + channel + "\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
	}

	if (mode.size() && !isModeValid(mode)) {
		std::string serverResponse = getServerReply(ERR_UNKNOWNMODE, clientSocket);
		serverResponse += " " + mode + " :Unknow mode\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return ;
	}

	if (isOperator("@" + userInfo_[clientSocket].nickname, channel)) {
		std::string modeResponse = ":" + userInfo_[clientSocket].server_addr + " ";
		modeResponse += "MODE " + channel + " " + mode;
		if (mode_option.size())
			modeResponse += " " + mode_option;
		modeResponse += "\r\n";
		printResponse(BROADCAST, modeResponse);
		printResponse(SERVER, modeResponse);
		send(clientSocket, modeResponse.c_str(), modeResponse.size(), 0);
		broadcastMessage(clientSocket, modeResponse, channel);
	}
}

void IRCServer::handleModeCommand(int clientSocket, std::istringstream & lineStream) {
	//:NICKNAME!USER@HOST MODE <channel|user> <modes> <modeparams>

	std::string channel_user, mode, mode_option;
	lineStream >> channel_user >> mode >> mode_option;

	if (isUser(channel_user)) {
		updateUserMode(clientSocket, channel_user, mode);
	} else {//if ischannel//{
		updateChannelMode(clientSocket, channel_user, mode, mode_option);
	} //else


    // Mode handling implementation goes here
}
