/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:27:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/20 16:22:35 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

/* ---------- Canonical Form ---------- */
Server::Server()
 : _defined_port("default"), _defined_password("default")
{
	if (DEBUG)
		printDebug("Server-> Default constructor called");
}

Server::Server(const Server& other)
 : _defined_port(other._defined_port), _defined_password(other._defined_password)
{
	if (DEBUG)
		printDebug("Server-> Copy constructor called");
}

Server::~Server()
{
	if (DEBUG)
		printDebug("Server-> Default destructor called");
}

Server& Server::operator=(const Server& other)
{
	_defined_port = other._defined_port;
	_defined_password = other._defined_password;
	return *this;
}

/* ---------- Methods ---------- */
void Server::init(char **argv)
{
	if (DEBUG)
		log("Server-> init() called");

	_defined_port = argv[1];
	_defined_password = argv[2];
	
	std::cout << "DEBUG: port = " << _defined_port << std::endl;
	std::cout << "DEBUG: password = " << _defined_password << std::endl;
	if (!atoi(_defined_port.c_str()) || atoi(_defined_port.c_str()) < 0)
	{
		log("vishhh");
	}
	
}