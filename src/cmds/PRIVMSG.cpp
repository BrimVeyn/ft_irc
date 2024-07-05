/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 15:30:56 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/05 16:26:30 by albeninc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


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

void IRCServer::handleFileCommand(int clientSocket, const std::string& message) {
    std::istringstream lineStream(message);
    std::string target;
    std::string fileName;
    lineStream >> target >> fileName;

    if (!target.empty()) {
        target = target.substr(1);
    }


    if (target.empty() || fileName.empty()) {
        std::string errorMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + target + " :Usage: FILE <target> <filename>\r\n";
        send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
        return;
    }

    std::ifstream file(fileName.c_str(), std::ios::binary);
    if (!file) {
        std::string errorMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + target + " :Could not open file\r\n";
        send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
        return;
    }

    std::vector<char> fileData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::string directory = "./received_files/";
    mkdir(directory.c_str(), 0777);

    std::string filePath = directory + fileName;
    std::ofstream outFile(filePath.c_str(), std::ios::binary);
    if (!outFile) {
        std::string errorMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + target + " :Could not write file to directory\r\n";
        send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
        return;
    }

    outFile.write(&fileData[0], fileData.size());
    outFile.close();

    std::ostringstream convert;
    convert << clientSocket;
    std::string clientSocketStr = convert.str();

    std::string successMessageToSender = getCommandPrefix(clientSocket) + "PRIVMSG " + clientSocketStr + " :File " + fileName + " stored successfully in " + filePath + "\r\n";
    send(clientSocket, successMessageToSender.c_str(), successMessageToSender.length(), 0);

}

void IRCServer::handlePrivmsgCommand(int clientSocket, std::istringstream &lineStream) {
    std::string target;
    lineStream >> target;
    std::string privmsg;
    getline(lineStream, privmsg);

    std::string formattedMessage = getCommandPrefix(clientSocket) + "PRIVMSG " + target + " " + privmsg + "\r\n";

    if (target[0] == '#') {
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
            handleGameCommand(clientSocket, privmsg);
        } else if (target == "FILE") {
            handleFileCommand(clientSocket, privmsg.substr(1));
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