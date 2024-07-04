/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 17:27:43 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 17:27:46 by albeninc         ###   ########.fr       */
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
