/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 10:05:00 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 15:42:16 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

static std::string addNumberToStr(const std::string & nickname, int nb) {
	std::stringstream ss;

	if (nb != 0)
		ss << nickname << nb;
	else
		ss << nickname;

	std::string str;
	ss >> str;

	return str;
}

void IRCServer::sendWelcomeMessages(int clientSocket) {
    userInfo &user = userInfo_[clientSocket];

    std::string welcome = SERVER_NAME  "001 " + user.nickname + " :Welcome to the IRC network, " + user.nickname + "!" + "\r\n";
    std::string yourHost = SERVER_NAME "002 " + user.nickname + " :Your host is --ft_irc-- running version 1.0" + "\r\n";
    std::string created = SERVER_NAME "003 " + user.nickname + " :This server was created on " + creationDate_ + "\r\n";
    std::string myInfo = SERVER_NAME"004 " + user.nickname + SERVER_NAME + "1.0 @bvan-pae/@nbardabi/@rrettien" + "\r\n";

	printResponse(SERVER, welcome);
	printResponse(SERVER, yourHost);
	printResponse(SERVER, created);
	printResponse(SERVER, myInfo);
    send(clientSocket, welcome.c_str(), welcome.size(), 0);
    send(clientSocket, yourHost.c_str(), yourHost.size(), 0);
    send(clientSocket, created.c_str(), created.size(), 0);
    send(clientSocket, myInfo.c_str(), myInfo.size(), 0);
}

void IRCServer::authenticateClient(int clientSocket) {
    userInfo &user = userInfo_[clientSocket];

    if (!user.password.empty() && !user.nickname.empty() && !user.username.empty() && !user.is_authenticated) {
        if (user.password == password_) {
            user.is_authenticated = true;
            std::cout << "Client " << clientSocket << " authenticated successfully." << std::endl;
            sendWelcomeMessages(clientSocket);
        } else {
            std::string response = SERVER_NAME "464 " + user.nickname + " :Wrong password\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
            closeSocket(clientSocket);
            std::cerr << "Client " << clientSocket << " provided incorrect password." << std::endl;
        }
    }
}

void IRCServer::handleUsernameCollision(int clientSocket, std::string & username) {
	int suffix = 0;
	for (std::vector<int>::iterator it = clients_.begin(); it != clients_.end(); it++) {
		if (*it == clientSocket) {
			continue;
		}
		if (userInfo_[*it].username == addNumberToStr(username, suffix)) {
			suffix += 1;
        }
	}
	if (suffix != 0)
		username = addNumberToStr(username, suffix);
}

// Gestion de la commande USER
void IRCServer::handleUserCommand(int clientSocket, std::istringstream & lineStream) {
    std::string username, hostname, servername, realname;

	// ----- Recupere info de lineStream ----- //
    lineStream >> username >> hostname >> servername;
    std::getline(lineStream, realname);
	userInfo_[clientSocket].server_addr = servername;

	handleUsernameCollision(clientSocket, username);
	
	if (filter(username) == false){
		std::string invalidNickname = getServerReply(ERR_ERRONEUSNICKNAME, clientSocket);
		invalidNickname += " " + username + " :Erroneus username\r\n";
		printResponse(SERVER, invalidNickname);
		send(clientSocket, invalidNickname.c_str(), invalidNickname.size(), 0);
		return ;
	}

    userInfo_[clientSocket].username = username;
	// -------------------------------------- //

	// ----- Gestion premiere connection ----- //
	if (userInfo_[clientSocket].is_authenticated == false){
		authenticateClient(clientSocket);
	}
	// -------------------------------------- //
}
