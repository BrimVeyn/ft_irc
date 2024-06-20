/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 11:05:34 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/06/20 17:09:59 by bvan-pae         ###   ########.fr       */
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
	IRCServer* ptr = new IRCServer(port, password);

	ptr->start();
	delete ptr;	
}
