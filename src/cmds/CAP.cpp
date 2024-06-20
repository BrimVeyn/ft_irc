/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CAP.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:56:49 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/20 11:24:10 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handleCapCommand(int clientSocket, std::istringstream & lineStream) {
	std::string subcommand;
	lineStream >> subcommand;
	
	if (subcommand == "LS" || subcommand == "LIST") {
		std::string response = "CAP * LS :\r\n";
		send(clientSocket, response.c_str(), response.size(), 0);
	} else if (subcommand == "END") {
		IRCServer::handleMotdCommand(clientSocket, lineStream);
	}
}
