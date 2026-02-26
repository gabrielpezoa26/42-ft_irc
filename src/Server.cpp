/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:27:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/26 19:02:45 by gcesar-n         ###   ########.fr       */
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


bool Server::g_continue_running = true;

void Server::signalHandler(int signum)
{
	(void)signum;
	Server::g_continue_running = 0;
}

void Server::setupSignals()
{
	struct sigaction sa;
	
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = Server::signalHandler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
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

	_add.sin_family = AF_INET;
	_add.sin_port = htons(_defined_port);
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	_add.sin_addr.s_addr = INADDR_ANY;
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
	memset(&_new_client, 0, sizeof(_new_client));
	_new_client.fd = _server_socket;
	_new_client.events = POLLIN;

	debugVar("_new_client events:", _new_client.events);
	debugVar("_new_client fds:", _new_client.fd);
	_vec_client_fds.push_back(_new_client);
}

void Server::run()
{
	if (DEBUG)
		printDebug("Server-> run() called");
	printCurrentTime();

	// int client_socket = -1;
	// socklen_t len;
	// struct sockaddr_in cli_container;
	
	// log("Server is running....");
	// len = sizeof(cli_container);
	// client_socket = accept(_server_socket, (struct sockaddr *)&cli_container, &len);  //temporário
	// _new_client.fd = client_socket;
	setupSignals();
	// char client_message[1024] = { 0 };  //temporário
	while (g_continue_running)
	{
		int poll_result = poll(&_vec_client_fds[0], _vec_client_fds.size(), -1);
		if (poll_result == -1 && Server::g_continue_running == true)
		{
			throw std::runtime_error("deu ruim");
		}
		for (size_t i = 0; i < _vec_client_fds.size(); i++)
		{
			if (_vec_client_fds[i].revents & POLLIN)
			{
				if (_vec_client_fds[i].fd == _server_socket)
				{
					log("1");
					_handleNewConnection();
				}
				else
				{
					log("2");
					_handleClientActivity(_vec_client_fds[i].fd);
				}
			}
		}
	}
	if (_server_socket != -1)
		close(_server_socket);
	log("\nsocket closeddd");
}

//temporario
void Server::_handleNewConnection()
{
	if (DEBUG)
		printDebug("Server-> _handleNewConnection() called");

	struct sockaddr_in client_address;
	socklen_t len = sizeof(client_address);

	int client_socket = accept(_server_socket, (struct sockaddr *)&client_address, &len);

	struct pollfd new_client_poll;
	new_client_poll.fd = client_socket;
	new_client_poll.events = POLLIN;
	new_client_poll.revents = 0;

	_vec_client_fds.push_back(new_client_poll);
	printDebug("conexao deu bom");
}

// isso eh temporário, dps tem q criar/usar a classe do Client e seus atributos
void Server::_handleClientActivity(int client_fd)
{
	if (DEBUG)
		printDebug("Server-> _handleClientActivity called");

	char client_message[1024];
	memset(client_message, 0, sizeof(client_message));

	ssize_t bytes_received = recv(client_fd, client_message, sizeof(client_message) - 1, 0);
	if (bytes_received > 0)
	{
		std::cout << "Client <" << client_fd << ">: " << client_message;
	}
	else
	{
		if (bytes_received == 0)
			std::cout << "Client <" << client_fd << "> disconnected" << std::endl;
		else
			std::cerr << "Error: connection lost on client <" << client_fd << ">." << std::endl;
		close(client_fd);
		for (std::vector<struct pollfd>::iterator it = _vec_client_fds.begin(); it != _vec_client_fds.end(); ++it)
		{
			if (it->fd == client_fd)
			{
				_vec_client_fds.erase(it);
				break; 
			}
		}
	}
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
