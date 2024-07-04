/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getters.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 17:26:51 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 17:26:55 by albeninc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/IRCServer.hpp"

bool IRCServer::isOperator(std::string nickname, std::string channel) {
	std::vector<std::string> operators = channelInfo_[channel].operators;
	if (std::find(operators.begin(), operators.end(), nickname) != operators.end())
		return true;
	return false;
}

bool IRCServer::isModeValid(std::string mode) {
	if (availableModes_.find(mode) != std::string::npos)
		return true;
	return false;
}

bool IRCServer::isUser(const std::string & str) {
	std::map<int, userInfo>::iterator it = userInfo_.begin();
	for (; it != userInfo_.end(); it++){
		if (it->second.nickname == str)
			return true;
	}
	return false;
}

int	IRCServer::getClientSocketFromUsername(const std::string &username) {
	std::map<int, userInfo>::iterator it = userInfo_.begin();
	for (; it != userInfo_.end(); it++){
		if (it->second.username == username)
			return it->first;
	}
	return -1;
}
