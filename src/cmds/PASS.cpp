/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PASS.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 17:27:37 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 17:27:39 by albeninc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handlePassCommand(int clientSocket, std::istringstream & lineStream) {
	std::string receivedPassword;
	lineStream >> receivedPassword;

	userInfo_[clientSocket].password = receivedPassword;
}
