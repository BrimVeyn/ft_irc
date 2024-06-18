/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getCommandPrefix.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 16:12:39 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/18 16:15:13 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"

std::string IRCServer::getCommandPrefix(int clientSocket) {
	std::string str;

	str += ":" + userInfo_[clientSocket].nickname;
	str += "!" + userInfo_[clientSocket].username;
	str += "@" + userInfo_[clientSocket].server_addr;
	str += " ";

	return str;
}
