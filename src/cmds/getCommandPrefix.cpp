/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getCommandPrefix.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 17:26:44 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 17:26:45 by albeninc         ###   ########.fr       */
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
