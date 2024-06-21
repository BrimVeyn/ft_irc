/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 10:05:26 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 11:06:22 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <cstdlib>
#include <sstream>

void IRCServer::updateUserMode(int clientSocket, std::string user, std::string mode) {
	std::string modeResponse = getCommandPrefix(clientSocket);
	modeResponse += "MODE " + user + " " + mode + "\r\n";
	printResponse(SERVER, modeResponse);
	send(clientSocket, modeResponse.c_str(), modeResponse.size(), 0);
}

void IRCServer::keyModeManager(int clientSocket, std::string channel, std::string mode, std::string mode_option) {
	if (!channelInfo_[channel].key.size() && mode == "+k") {
		if (mode_option.size()) {
			//Update key in db, and brodcast change key message
			channelInfo_[channel].key = mode_option;
			std::string modeResponse = SERVER_NAME;
			modeResponse += "MODE " + channel + " " + mode + " " + mode_option + "\r\n";
			printResponse(BROADCAST, modeResponse);
			printResponse(SERVER, modeResponse);
			send(clientSocket, modeResponse.c_str(), modeResponse.size(), 0);
			broadcastMessage(clientSocket, modeResponse, channel);
			return ;
        } else {
			//needmore params;
			std::string serverResponse = getServerReply(ERR_NEEDMOREPARAMS, clientSocket);
			serverResponse += " " + channel + " " + mode + " :Need a key\r\n";
			printResponse(SERVER, serverResponse);
			send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
			return ;
		}
	} else if (channelInfo_[channel].key.size() && mode == "+k") {
		//Error key is already set
		std::string serverResponse = getServerReply(ERR_KEYSET, clientSocket);
		serverResponse += " " + channel + " :Key is already set\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return ;
	} else if (channelInfo_[channel].key.size() && mode == "-k") {
		//Remove key from db and broadcast mode change
		channelInfo_[channel].key.erase();
		std::string modeResponse = SERVER_NAME;
		modeResponse += "MODE " + channel + " " + mode + "\r\n";
		printResponse(BROADCAST, modeResponse);
		printResponse(SERVER, modeResponse);
		send(clientSocket, modeResponse.c_str(), modeResponse.size(), 0);
		broadcastMessage(clientSocket, modeResponse, channel);
	}
}

void IRCServer::operatorModeManager(int clientSocket, std::string channel, std::string mode, std::string mode_option) {
	if (getMemberList(channel).find(mode_option) == std::string::npos) {
		//send error no such nick
		std::string serverResponse = getServerReply(ERR_NOSUCHNICK, clientSocket);
		serverResponse += " " + mode_option + " :No such nick in channel\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return ;
	}
	if (mode == "+o") {
		//put in operator
		removeMember(getClientSocket(mode_option), channel);
		channelInfo_[channel].operators.push_back("@" + mode_option);
	} else if (mode == "-o") {
		//put in member
		removeMember(getClientSocket(mode_option), channel);
		channelInfo_[channel].members.push_back(mode_option);
	}

	std::string modeResponse = SERVER_NAME;
	modeResponse += "MODE " + channel + " " + mode + " " + mode_option + "\r\n";
	printResponse(BROADCAST, modeResponse);
	printResponse(SERVER, modeResponse);
	send(clientSocket, modeResponse.c_str(), modeResponse.size(), 0);
	broadcastMessage(clientSocket, modeResponse, channel);
}

void IRCServer::userLimitModeManager(int clientSocket, std::string channel, std::string mode, std::string mode_option) {
	//Send error nee more params if +l without param
	if (mode == "+l" && !mode_option.size()) {
		std::string serverResponse = getServerReply(ERR_NEEDMOREPARAMS, clientSocket);
		serverResponse += " " + channel + " " + mode + " :Need a key\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return ;
	}
	
	//Set or reset userlimit
	if (mode == "+l") {
		channelInfo_[channel].userLimit = std::atoi(mode_option.c_str());
	} else if (mode == "-l") {
		channelInfo_[channel].userLimit = 0;
	}

	std::string modeResponse = SERVER_NAME;
	modeResponse += "MODE " + channel + " " + mode;
	if (mode_option.size())
		modeResponse += " " + mode_option;
	modeResponse += "\r\n";
	printResponse(BROADCAST, modeResponse);
	printResponse(SERVER, modeResponse);
	send(clientSocket, modeResponse.c_str(), modeResponse.size(), 0);
	broadcastMessage(clientSocket, modeResponse, channel);
}

void IRCServer::topicLockModeManager(int clientSocket, std::string channel, std::string mode) {
	//Update topicIsProtected
	if (mode == "+t") {
		channelInfo_[channel].isTopicProtected = true;
	} else if (mode == "-t") {
		channelInfo_[channel].isTopicProtected = false;
	}

	std::string modeResponse = SERVER_NAME;
	modeResponse += "MODE " + channel + " " + mode + "\r\n";
	printResponse(BROADCAST, modeResponse);
	printResponse(SERVER, modeResponse);
	send(clientSocket, modeResponse.c_str(), modeResponse.size(), 0);
	broadcastMessage(clientSocket, modeResponse, channel);
}

void IRCServer::inviteModeManager(int clientSocket, std::string channel, std::string mode) {
	if (mode == "+i") {
		channelInfo_[channel].isInviteOnly = true;
	} else if (mode == "-i") {
		channelInfo_[channel].isInviteOnly = false;
	}

	std::string modeResponse = SERVER_NAME;
	modeResponse += "MODE " + channel + " " + mode + "\r\n";
	printResponse(BROADCAST, modeResponse);
	printResponse(SERVER, modeResponse);
	send(clientSocket, modeResponse.c_str(), modeResponse.size(), 0);
	broadcastMessage(clientSocket, modeResponse, channel);
}

void IRCServer::updateChannelMode(int clientSocket, std::string channel, std::string mode, std::string mode_option) {
	if (!mode.size() && !mode_option.size()) {
		//Print current channel modes
		std::string serverResponse = getServerReply(RPL_CHANNELMODEIS, clientSocket);
		serverResponse += " " + channel;
		serverResponse += gatherModes(channel) + "\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return ;
	}

	if (mode.size() && !isModeValid(mode)) {
		//Send error, unknown mode
		std::string serverResponse = getServerReply(ERR_UNKNOWNMODE, clientSocket);
		serverResponse += " " + mode + " :Unknow mode\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return ;
	}

	if (isOperator("@" + userInfo_[clientSocket].nickname, channel)) {
		//Change mode
		modeSplitter(clientSocket, channel, mode, mode_option);
	} else {
		//Send error, channel privilege required
		std::string serverResponse = getServerReply(ERR_CHANOPRIVSNEEDED, clientSocket);
		serverResponse += " " + channel + " :Not enough privilege\r\n";
		printResponse(SERVER, serverResponse);
		send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);
		return ;
	}
}

std::string IRCServer::gatherModes(std::string channel) {
	channelInfo chan = channelInfo_[channel];
	std::string modesNParams;

	if (!chan.userLimit && !chan.isInviteOnly && !chan.isTopicProtected && !chan.key.size()) {
		return modesNParams;
	} else {
		std::stringstream uLimit;
		uLimit << chan.userLimit;
		modesNParams += " +";
		if (chan.userLimit) modesNParams += "l";
		if (chan.isInviteOnly) modesNParams += "i";
		if (chan.isTopicProtected) modesNParams += "t";
		if (chan.key.size()) modesNParams += "k " + chan.key;
		if (chan.userLimit) modesNParams += " " + uLimit.str();
	}
	return modesNParams;
}

void IRCServer::modeSplitter(int clientSocket, std::string channel, std::string mode, std::string mode_option) {
	switch (mode[1]) {
		case 'k':
			keyModeManager(clientSocket, channel, mode, mode_option);
			break;
		case 'o':
			operatorModeManager(clientSocket, channel, mode, mode_option);
			break;
		case 'l':
			userLimitModeManager(clientSocket, channel, mode, mode_option);
			break;
		case 't':
			topicLockModeManager(clientSocket, channel, mode);
			break;
		case 'i':
			inviteModeManager(clientSocket, channel, mode);
	};
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
}
