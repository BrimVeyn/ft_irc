/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   motd.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 09:36:16 by nbardavi          #+#    #+#             */
/*   Updated: 2024/06/20 12:07:10 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleMotdCommand(int clientSocket, std::istringstream& lineStream) {
	(void)lineStream;
	std::string subcommand;
	std::string addr = userInfo_[clientSocket].server_addr;
	std::string nick = userInfo_[clientSocket].nickname;
	std::string user = userInfo_[clientSocket].username;
	std::string s_name = "ft_irc";

	std::string welcomeMessage = ":" + s_name + " 001 " + nick + " :" + GREEN + "Welcome to the Internet Relay Network :" + nick + "!" + user + "@" + addr + RESET_COLOR + "\r\n";
	send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
	printResponse(SERVER, welcomeMessage);

	std::string infos = ":" + s_name + " 002 " + nick + " :" + GREEN + "Your host is " + s_name + ", running version 1.0" + RESET_COLOR + "\r\n";
	send(clientSocket, infos.c_str(), infos.size(), 0);
	printResponse(SERVER, infos);

	std::string date = ":" + s_name + " 003 " + nick + " :" + GREEN + "created on " + creationDate_ + RESET_COLOR + "\r\n";
	send(clientSocket, date.c_str(), date.size(), 0);
	printResponse(SERVER, date);
}
