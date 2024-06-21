/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 11:03:29 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/21 11:03:55 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"

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
