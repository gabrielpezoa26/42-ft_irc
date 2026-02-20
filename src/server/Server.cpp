/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:27:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/20 18:56:04 by gcesar-n         ###   ########.fr       */
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
	std::string valid_chars = "0123456789";
	if (_defined_port.find_first_not_of(valid_chars) != std::string::npos)
		return false;
	if (!atoi(_defined_port.c_str()) || atoi(_defined_port.c_str()) < 0 || atol(_defined_port.c_str()) > std::numeric_limits<int>::max())
		return false;
	return true;
}

bool Server::isPasswordValid(const std::string &password)
{
	if (DEBUG)
		printDebug("Server-> isPasswordValid() called");

	_defined_password = password;
	if (_defined_password.empty())
		return false;
	for (size_t i = 0; i < _defined_password.length(); i++)
	{
		if (!isascii(_defined_password[i]))
			return false;
	}
	// if (atoi(_defined_password.c_str()) < 5)
	// 	return false;
	return true;
}
