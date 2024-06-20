/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:50:13 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/19 15:51:08 by nbardavi         ###   ########.fr       */
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

// Gestion de la commande USER
void IRCServer::handleUserCommand(int clientSocket, std::istringstream & lineStream) {
    std::string username, hostname, servername, realname;

	// ----- Recupere info de lineStream ----- //
    lineStream >> username >> hostname >> servername;
    std::getline(lineStream, realname);
	userInfo_[clientSocket].server_addr = servername;

	int suffix = 0;
	for (std::vector<int>::iterator it = clients_.begin(); it != clients_.end(); it++) {
		if (*it == clientSocket) {
			continue;
		}
		std::cout << GREEN "Comparing : " << userInfo_[*it].username << " " << username << RESET_COLOR << std::endl;
		if (userInfo_[*it].username == addNumberToStr(username, suffix)) {
			suffix += 1;
        }
	}
	if (suffix != 0)
		username = addNumberToStr(username, suffix);

    userInfo_[clientSocket].username = username;
	// -------------------------------------- //

	// ----- Gestion premiere connection ----- //
	if (userInfo_[clientSocket].is_register == false){
		userInfo_[clientSocket].is_register = true;

		std::string responseNICK = ":" + hostname + "!" + hostname + "@" + servername + " NICK :" + userInfo_[clientSocket].nickname + "\r\n";
		if (send(clientSocket, responseNICK.c_str(), responseNICK.size(), 0) == -1) {
			std::cerr << "Failed to send NICK response" << std::endl;
		} else {
			std::cerr << "Successfully sent NICK response" << std::endl;
		}
	}
	// -------------------------------------- //

	std::cout << MAGENTA << "username is " << username << RESET_COLOR << std::endl;
    // Example response (you might want to customize this)
    std::string response = ":localhost 001 " + userInfo_[clientSocket].nickname + " :Welcome to the IRC server\r\n";
    std::cout << "Handling USER command for client " << clientSocket << " with username " << username << std::endl;

    // Send response to the client
    send(clientSocket, response.c_str(), response.size(), 0);
}
