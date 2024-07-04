/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albeninc <albeninc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 17:28:11 by albeninc          #+#    #+#             */
/*   Updated: 2024/07/04 17:28:13 by albeninc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/IRCServer.hpp"
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <csignal>

int main(int ac, char *av[]) {

	if (ac != 3) {
		std::cout << "Usage : " << av[0] << " <port number> <password>" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	const std::string password(av[2]);
	int port = std::atoi(av[1]);
	if ( port < 1024 || port > 49151){
		std::cerr << RED << "Error: Port need to be: 1024 < port < 49151" << RESET_COLOR << std::endl;
		exit(EXIT_FAILURE);
	}

	IRCServer* ptr = new IRCServer(port, password);

	ptr->start();
	delete ptr;	
}
