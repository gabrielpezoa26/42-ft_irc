/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:27:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/22 11:47:18 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

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
}

void Server::setSocket()
{
	if (DEBUG)
		printDebug("Server-> setSocket() called");

	_add.sin_family = AF_INET;  //assim funciona só com ipv4, dps TALVEZ tem q mudar pra funfar com ipv6 tbm.
	_add.sin_port = htons(_defined_port);
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	int flag = 1;

	if (_server_socket == -1)
		throw std::runtime_error("Error while socket creation");
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
		throw std::runtime_error("Error while setting SO_REUSEADDR on main socket");
	if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error while setting O_NONBLOCK on main socket");
	if (bind(_server_socket, (struct sockaddr *)&_add, sizeof(_add)) == -1)
		throw std::runtime_error("Error while binding socket");
	if (listen(_server_socket, SOMAXCONN) == -1)
		throw std::runtime_error("Error while listen() call");

	// debugVar("port", _defined_port);
	// debugVar("password", _defined_password);
	// debugVar("socket fd", _server_socket);
	// debugVar("_add.sin_family", _add.sin_family);
	// debugVar("_add.sin_port", _add.sin_port);
	// debugVar("SOMAXCONN macro value", SOMAXCONN);
}

void Server::run()
{
	if (DEBUG)
		printDebug("Server-> run() called");
	
	std::time_t start_time = std::time(0);
	char* readable_start_time = std::ctime(&start_time);
	std::cout << GREEN << readable_start_time << RESET;

	int client_socket;
	int len;
	struct sockaddr_in cli;

	len = sizeof(cli);
	client_socket = accept(_server_socket, NULL, NULL); //o erro ta aki
	debugVar("client socket", client_socket);

	
	log("Server is running....");
	// loop q faz o parse dos sinais/comandos/etc
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
