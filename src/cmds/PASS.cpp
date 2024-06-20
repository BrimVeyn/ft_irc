/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PASS.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/17 16:58:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/20 14:52:54 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"
#include <sstream>

void IRCServer::handlePassCommand(int clientSocket, std::istringstream & lineStream) {
	std::string receivedPassword;
	lineStream >> receivedPassword;

	userInfo_[clientSocket].password = receivedPassword;
}
