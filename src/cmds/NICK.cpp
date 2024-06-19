/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 13:25:04 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/19 10:47:23 by nbardavi         ###   ########.fr       */
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

	for (std::vector<int>::iterator it = clients_.begin(); it != clients_.end();) {
		if (*it == clientSocket) {
			std::cout << "hello";
			it++;
			continue;
		}
		std::cout << GREEN "Comparing : " << userInfo_[*it].nickname << nickname << RESET_COLOR << std::endl;
		if (userInfo_[*it].nickname == nickname) {
			nickname += "_";
			it = clients_.begin();
        } else 
			it++;
	}

	// if (suffix != 0)
	// 	nickname = addNumberToStr(nickname, suffix);
   

	userInfo_[clientSocket].nickname = nickname;
	// if (userInfo_[clientSocket].is_register == false){
	// 	return;
	// }
	std::string response = getCommandPrefix(clientSocket) + "NICK :" + nickname + "\r\n";

	std::cout << YELLOW << response << RESET_COLOR << std::endl;
    
    // Send the NICK response to the client
    if (send(clientSocket, response.c_str(), response.size(), 0) == -1) {
        std::cerr << "Failed to send NICK response" << std::endl;
    } else {
        std::cerr << "Successfully sent NICK response" << std::endl;
    }
}
