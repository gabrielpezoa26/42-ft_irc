/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:27:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/20 17:33:53 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

/* ---------- Canonical Form ---------- */
Server::Server()
: _defined_port("default_port"), _defined_password("default_password")
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
		printDebug("Server-> init() called");

	_defined_port = argv[1];
	_defined_password = argv[2];
	if (!isValidPort(_defined_port) || !isPasswordValid(_defined_password))
	{
		throw std::runtime_error("Server-> Exception caught: invalid input");
	}
	log("DEBUG: init-> deu bommm");
}

bool Server::isValidPort(const std::string &port)
{
	if (DEBUG)
		printDebug("Server-> isValidPort() called");

	_defined_port = port;
	// std::cout << "DEBUG: port = " << _defined_port << std::endl;
	if (!atoi(_defined_port.c_str()) || atoi(_defined_port.c_str()) < 0
		|| atol(_defined_port.c_str()) > std::numeric_limits<int>::max())
	{
		// log("invalid port");
		return false;
	}
	return true;
}

bool Server::isPasswordValid(const std::string &password)
{
	if (DEBUG)
		printDebug("Server-> isPasswordValid() called");

	_defined_password = password;
	// std::cout << "DEBUG: password = " << _defined_password << std::endl;
	if (_defined_password.empty())
	{
		// log("password cannot be empty");
		return false;
	}
	for (size_t i = 0; i < _defined_password.length(); i++)
	{
		if (!isascii(_defined_password[i]))
		{
			// log("DEBUG: senha nao eh ascii");
			return false;
		}
	}
	return true;
}
