/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getCommandPrefix.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 16:12:39 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/19 11:55:17 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/IRCServer.hpp"

std::string IRCServer::getCommandPrefix(int clientSocket) {
	std::string str;

	std::cout << userInfo_[clientSocket].nickname << std::endl;
	str += ":" + userInfo_[clientSocket].nickname;
	str += "!" + userInfo_[clientSocket].username;
	str += "@" + userInfo_[clientSocket].server_addr;
	str += " ";

	return str;
}
