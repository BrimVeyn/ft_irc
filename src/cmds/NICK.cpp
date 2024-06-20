/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 13:25:04 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/20 09:55:36 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

std::string addNumberToStr(const std::string & nickname, int nb) {
	std::stringstream ss;

	if (nb != 0)
		ss << nickname << nb;
	else
		ss << nickname;

	std::string str;
	ss >> str;

	return str;
}

// Gestion de la commande NICK
void IRCServer::handleNickCommand(int clientSocket, std::istringstream & lineStream) {
	std::string nickname;
	lineStream >> nickname;

	// ----- Ajout suffix si duplication de nickname ----- //
	int suffix = 0;
	for (std::vector<int>::iterator it = clients_.begin(); it != clients_.end(); it++) {
		if (*it == clientSocket) {
			continue;
		}
		std::cout << GREEN "Comparing : " << userInfo_[*it].nickname << " " << nickname << RESET_COLOR << std::endl;
		if (userInfo_[*it].nickname == addNumberToStr(nickname, suffix)) {
			suffix += 1;
        }
	}
	if (suffix != 0)
		nickname = addNumberToStr(nickname, suffix);
	//-----------------------------------------//

	// ----- Gestion premiere connection ----- //
	if (userInfo_[clientSocket].is_register == false){
		userInfo_[clientSocket].nickname = nickname;
		return;
	}
	//-----------------------------------------//

	std::string response = getCommandPrefix(clientSocket) + "NICK :" + nickname + "\r\n";
	userInfo_[clientSocket].nickname = nickname;

	std::cout << YELLOW << response << RESET_COLOR << std::endl;
    
    // Send the NICK response to the client
    if (send(clientSocket, response.c_str(), response.size(), 0) == -1) {
        std::cerr << "Failed to send NICK response" << std::endl;
    } else {
        std::cerr << "Successfully sent NICK response" << std::endl;
    }
	//-----------------------------------------//
}
