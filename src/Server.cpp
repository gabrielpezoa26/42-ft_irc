/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:27:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/11 15:49:01 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

/* ---------- Canonical Form ---------- */
Server::Server()
: _server_port(0), _server_password("default_password")
{
	if (DEBUG_SERVER)
		printDebug("Server-> Default constructor called");
}

Server::Server(const Server& other)
: _server_port(other._server_port), _server_password(other._server_password)
{
	if (DEBUG_SERVER)
		printDebug("Server-> Copy constructor called");
}

Server::~Server()
{
	if (DEBUG_SERVER)
		printDebug("Server-> Default destructor called");
}

Server& Server::operator=(const Server& other)
{
	if (DEBUG_SERVER)
		printDebug("Server-> assign operator called");

	if (this != &other)
	{
		_server_port = other._server_port;
		_server_password = other._server_password;
	}
	return *this;
}

bool Server::_continue_running = true;

void Server::handleSignals(int signum)
{
	(void)signum;
	Server::_continue_running = 0;
}

void Server::setupSignals()
{
	struct sigaction sa;
	
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = Server::handleSignals;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

/* ---------- Methods ---------- */
void Server::init(char **argv)
{
	if (DEBUG_SERVER)
		printDebug("Server-> init() called");

	std::string input_port = argv[1];
	_server_password = argv[2];
	if (!_isValidPort(input_port) || !_isValidPassword(_server_password))
	{
		throw std::invalid_argument("Server-> Exception caught: invalid input");
	}
}

void Server::setSocket()
{
	if (DEBUG_SERVER)
		printDebug("Server-> setSocket() called");

	_server_adress.sin_family = AF_INET;
	_server_adress.sin_port = htons(_server_port);
	_server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	_server_adress.sin_addr.s_addr = INADDR_ANY;
	int flag = 1;

	if (_server_socket_fd == -1)
		throw std::runtime_error("Error while socket creation");
	if (setsockopt(_server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
		throw std::runtime_error("Error while setting SO_REUSEADDR on main socket");
	if (fcntl(_server_socket_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error while setting O_NONBLOCK on main socket");
	if (bind(_server_socket_fd, (struct sockaddr *)&_server_adress, sizeof(_server_adress)) == -1)
		throw std::runtime_error("Error while binding socket");
	if (listen(_server_socket_fd, SOMAXCONN) == -1)
		throw std::runtime_error("Error while listen() call");
	memset(&_new_client_poll, 0, sizeof(_new_client_poll));
	_new_client_poll.fd = _server_socket_fd;
	_new_client_poll.events = POLLIN;

	_vec_client_fds.push_back(_new_client_poll);
}

void Server::run()
{
	if (DEBUG_SERVER)
		printDebug("Server-> run() called");
	
	printCurrentTime();
	setupSignals();
	while (_continue_running)
	{
		int poll_result = poll(&_vec_client_fds[0], _vec_client_fds.size(), -1);
		if (poll_result == -1 && Server::_continue_running == true)
		{
			throw std::runtime_error("deu ruim");
		}
		for (size_t i = 0; i < _vec_client_fds.size(); i++)
		{
			if (_vec_client_fds[i].revents & POLLIN)
			{
				if (_vec_client_fds[i].fd == _server_socket_fd)
				{
					_handleNewConnection();
				}
				else
				{
					_handleClientActivity(_vec_client_fds[i].fd);
				}
			}
		}
	}
	if (_server_socket_fd != -1)
		close(_server_socket_fd);
	log("\nsocket closeddd");
}

void Server::_handleNewConnection()
{
	if (DEBUG_SERVER)
		printDebug("Server-> _handleNewConnection() called");

	Client client;
	socklen_t len = sizeof(_client_address);
	memset(&_client_address, 0, sizeof(_client_address));

	int client_socket = accept(_server_socket_fd, (struct sockaddr *)&_client_address, &len);
	if (client_socket == -1)
	{
		log("vishhh no accept()");
		return ;
	}
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		log("vishh no fcntl()");
		return ;
	}
	_new_client_poll.fd = client_socket;
	_new_client_poll.events = POLLIN;
	_new_client_poll.revents = 0;
	_vec_client_fds.push_back(_new_client_poll);

	client.setClientFd(client_socket);
	_map_connected_clients[client_socket] = client;
	printDebug("conexao deu bom");
}

//refatorar dps
void Server::_handleClientActivity(int client_fd)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _handleClientActivity called");

	std::map<int, Client>::iterator it = _map_connected_clients.find(client_fd);
	if (it == _map_connected_clients.end())
	{
		log("vishhh: client not found in map");
		return;
	}
	char client_message[1024];
	memset(client_message, 0, sizeof(client_message));
	ssize_t bytes_received = recv(client_fd, client_message, sizeof(client_message) - 1, 0);
	if (bytes_received > 0)
	{
		std::string new_data(client_message, bytes_received);
		it->second.appendInputBuffer(new_data);
		while (true)
		{
			std::string extracted_cmd = it->second.fetchCommand();
			if (extracted_cmd.empty())
				break;
			std::cout << "Client <" << client_fd << "> sent: [" << extracted_cmd << "]" << std::endl;
		}
	}
	else
	{
		if (bytes_received == 0)
			std::cout << "Client <" << client_fd << "> disconnected" << std::endl;
		else
			std::cerr << "Error: connection lost on client <" << client_fd << ">." << std::endl;
		close(client_fd);
		_map_connected_clients.erase(client_fd);
		for (std::vector<struct pollfd>::iterator poll_it = _vec_client_fds.begin(); poll_it != _vec_client_fds.end(); ++poll_it)
		{
			if (poll_it->fd == client_fd)
			{
				_vec_client_fds.erase(poll_it);
				break;
			}
		}
	}
}

/* ---------- Helpers ---------- */
bool Server::_isValidPort(const std::string &port)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _isValidPort() called");

	std::string temp = port;
	std::string valid_chars = "0123456789";
	if (temp.find_first_not_of(valid_chars) != std::string::npos)
		return false;
	if (!atoi(temp.c_str()) || atoi(temp.c_str()) < 0 || atol(temp.c_str()) > std::numeric_limits<int>::max())
		return false;
	_server_port = atoi(temp.c_str());
	if (_server_port < 1024 || _server_port > 65535)  // tem q tratar se n falha na htons() ou bind()
		return false;
	return true;
}

bool Server::_isValidPassword(const std::string &password)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _isValidPassword() called");

	_server_password = password;
	if (_server_password.empty())
		return false;
	for (size_t i = 0; i < _server_password.length(); i++)
	{
		if (!isascii(_server_password[i]))
			return false;
	}
	return true;
}
