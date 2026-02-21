/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:27:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/21 15:09:22 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

/* ---------- Canonical Form ---------- */
Server::Server()
: _defined_port(0), _defined_password("default_password")
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

	std::string input_port = argv[1];
	_defined_password = argv[2];
	if (!_isValidPort(input_port) || !_isValidPassword(_defined_password))
	{
		throw std::invalid_argument("Server-> Exception caught: invalid input");
	}
	printVarDebug("port", _defined_port);
	printVarDebug("password", _defined_password);
}

void Server::setSocket()
{
	if (DEBUG)
		printDebug("Server-> setSocket() called");

	add.sin_family = AF_INET;  //funciona só com ipv4, dps tem q mudar
	add.sin_port = htons(_defined_port);
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	int flag = 1;

	if (_socket_fd == -1)
		throw std::runtime_error("Error while socket creation");
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR,  &flag, sizeof(flag)) == -1)
		throw std::runtime_error("Error when setting SO_REUSEADDR on main socket");
	if (fcntl(_socket_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error when setting O_NONBLOCK on main socket");


	printVarDebug("socket fd", _socket_fd);
}



/* ---------- Helpers ---------- */
bool Server::_isValidPort(const std::string &port)
{
	if (DEBUG)
		printDebug("Server-> _isValidPort() called");

	std::string temp = port;
	std::string valid_chars = "0123456789";
	if (temp.find_first_not_of(valid_chars) != std::string::npos)
		return false;
	if (!atoi(temp.c_str()) || atoi(temp.c_str()) < 0 || atol(temp.c_str()) > std::numeric_limits<int>::max())
		return false;
	
	_defined_port = atoi(temp.c_str());
	return true;
}

bool Server::_isValidPassword(const std::string &password)
{
	if (DEBUG)
		printDebug("Server-> _isValidPassword() called");

	_defined_password = password;
	if (_defined_password.empty())
		return false;
	for (size_t i = 0; i < _defined_password.length(); i++)
	{
		if (!isascii(_defined_password[i]))
			return false;
	}
	return true;
}
