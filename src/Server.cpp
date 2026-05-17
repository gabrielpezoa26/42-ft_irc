/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:27:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/17 00:53:24 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

/* ---------- Canonical Form ---------- */
Server::Server()
: _server_port(0), _server_password("default_password"),_auth_handler(_map_connected_clients), 
  _command_handler(_map_connected_clients, _map_channels)
{
	if (DEBUG_SERVER)
		printDebug("Server-> Default constructor called");
}

Server::Server(const Server& other)
: _server_port(other._server_port), _server_password(other._server_password), _auth_handler(_map_connected_clients),
  _command_handler(_map_connected_clients, _map_channels)
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
	if (_server_port < 1024 || _server_port > 65535)
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

void Server::_closeFds()
{
	for (std::map<int, Client>::iterator it = _map_connected_clients.begin(); it != _map_connected_clients.end(); ++it)
	{
		if (it->first != -1)
			close(it->first);
	}
	_map_connected_clients.clear();
	_vec_client_fds.clear();
	if (_server_socket_fd != -1)
		close(_server_socket_fd);
	
}

/* ---------- Signals ---------- */
bool Server::_continue_running = true;

void Server::_handleSignals(int signum)
{
	(void)signum;
	Server::_continue_running = 0;
}

void Server::setupSignals()
{
	struct sigaction sa;
	
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = Server::_handleSignals;
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

	_server_address.sin_family = AF_INET;
	_server_address.sin_port = htons(_server_port);
	_server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	_server_address.sin_addr.s_addr = INADDR_ANY;
	int flag = 1;

	if (_server_socket_fd == -1)
		throw std::runtime_error("Error while socket creation");
	if (setsockopt(_server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
		throw std::runtime_error("Error while setting SO_REUSEADDR on main socket");
	if (fcntl(_server_socket_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error while setting O_NONBLOCK on main socket");
	if (bind(_server_socket_fd, (struct sockaddr *)&_server_address, sizeof(_server_address)) == -1)
		throw std::runtime_error("Error while binding socket");
	if (listen(_server_socket_fd, SOMAXCONN) == -1)
		throw std::runtime_error("Error while listen() call");
	memset(&_new_client_poll, 0, sizeof(_new_client_poll));
	_new_client_poll.fd = _server_socket_fd;
	_new_client_poll.events = POLLIN;
	_vec_client_fds.push_back(_new_client_poll);
}

void Server::_prepareEvents()
{
	for (size_t i = 0; i < _vec_client_fds.size(); i++)
	{
		if (_vec_client_fds[i].fd == _server_socket_fd)
			continue;
		
		std::map<int, Client>::iterator it = _map_connected_clients.find(_vec_client_fds[i].fd);
		if (it != _map_connected_clients.end())
		{
			if (!it->second.getOutputBuffer().empty())
				_vec_client_fds[i].events = POLLIN | POLLOUT;
			else
				_vec_client_fds[i].events = POLLIN;
		}
	}
}

void Server::_handleClientWrite(int client_fd)
{
	std::map<int, Client>::iterator it = _map_connected_clients.find(client_fd);
	if (it != _map_connected_clients.end())
	{
		const std::string& message = it->second.getOutputBuffer();
		if (DEBUG_WRITE)
			std::cout << PURPLE << "DEBUG WRITE [fd=" << client_fd << "] buffer='" << message << "'" << RESET <<std::endl;
		
		ssize_t bytes_sent = send(it->first, message.c_str(), message.length(), 0);

		if (DEBUG_WRITE)
			std::cout << PURPLE << "DEBUG WRITE [fd=" << client_fd << "] bytes_sent=" << bytes_sent << RESET << std::endl;
		if (bytes_sent > 0)
			it->second.eraseOutputBuffer(bytes_sent);
		else if (bytes_sent == -1)
			printError("Failed to send data");
	}
}

void Server::_processEvents()
{
	if (DEBUG_SERVER)
		printDebug("Server-> _processEvents() called");
	for (size_t i = 0; i < _vec_client_fds.size(); i++)
	{
		if (_vec_client_fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
		{
			_disconnectClient(_vec_client_fds[i].fd);
			i--;
			continue;
		}
		if (_vec_client_fds[i].revents & POLLIN)
		{
			if (_vec_client_fds[i].fd == _server_socket_fd)
				_handleNewConnection();
			else
			{
				if (!_handleClientActivity(_vec_client_fds[i].fd))
				{
					i--;
					continue;
				}
			}
		}
		if (i < _vec_client_fds.size() && (_vec_client_fds[i].revents & POLLOUT))
		{
			_handleClientWrite(_vec_client_fds[i].fd);
			std::map<int, Client>::iterator it = _map_connected_clients.find(_vec_client_fds[i].fd);
			if (it != _map_connected_clients.end() && it->second.getIsQuitting() && it->second.getOutputBuffer().empty())
			{
				_disconnectClient(_vec_client_fds[i].fd);
				i--;
				continue;
			}
		}
	}
}

void Server::run()
{
	if (DEBUG_SERVER)
		printDebug("Server-> run() called");
	
	printCurrentTime();
	setupSignals();
	while (_continue_running)
	{
		_prepareEvents();
		int poll_result = poll(&_vec_client_fds[0], _vec_client_fds.size(), -1);
		if (poll_result == -1 && Server::_continue_running == true)
		{
			throw std::runtime_error("Error while polling");
		}
		_processEvents();
	}
	_closeFds();
}

void Server::_handleNewConnection()
{
	if (DEBUG_SERVER)
		printDebug("Server-> _handleNewConnection() called");

	socklen_t len = sizeof(_client_address);
	memset(&_client_address, 0, sizeof(_client_address));

	int client_socket = accept(_server_socket_fd, (struct sockaddr *)&_client_address, &len);
	if (client_socket == -1)
	{
		log("Error on accept()");
		return;
	}

	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		log("Error on fcntl()");
		close(client_socket);
		return;
	}

	_new_client_poll.fd = client_socket;
	_new_client_poll.events = POLLIN;
	_new_client_poll.revents = 0;
	_vec_client_fds.push_back(_new_client_poll);

	Client client;
	client.setClientFd(client_socket);
	_map_connected_clients[client_socket] = client;
}

bool Server::_handleClientActivity(int client_fd)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _handleClientActivity() called");
	char client_message[1024];
	memset(client_message, 0, sizeof(client_message));
	ssize_t bytes_received = recv(client_fd, client_message, sizeof(client_message) - 1, 0);

	if (DEBUG_RECV)
	{
		if (bytes_received > 0)
		{
			std::string raw(client_message, bytes_received);
			std::cout << PURPLE << "RAW RECV [" << client_fd << "]: " << RESET;
			for (size_t i = 0; i < raw.size(); i++)
			{
				if (raw[i] == '\r')
					std::cerr << "\\r";
				else if (raw[i] == '\n')
					std::cerr << "\\n";
			else
				std::cerr << raw[i];
			}
			std::cerr << std::endl;
		}
	}
	if (bytes_received <= 0)
	{
		if (bytes_received != 0)
			std::cerr << "Error: connection lost on client <" << client_fd << ">." << std::endl;
		_disconnectClient(client_fd);
		return false;
	}
	std::string new_data(client_message, bytes_received);
	std::map<int, Client>::iterator it = _map_connected_clients.find(client_fd);
	if (it == _map_connected_clients.end())
		return false;
	it->second.appendInputBuffer(new_data);
	while (true)
	{
		it = _map_connected_clients.find(client_fd);
		if (it == _map_connected_clients.end())
			return false;
		std::string extracted_cmd = it->second.fetchCommand();
		if (extracted_cmd.empty())
			break;
		_routeCommand(it->second, extracted_cmd);
	}
	return true;
}

void Server::_splitCommand(const std::string& cmd, std::string& command, std::string& args)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _splitCommand() called");
	std::string clean_cmd = trim(cmd);
	if (clean_cmd.empty())
		return;

	std::string::size_type pos = clean_cmd.find(' ');
	if (pos == std::string::npos)
	{
		command = normalize(clean_cmd);
		args = "";
	}
	else
	{
		std::string tmp_cmd = clean_cmd.substr(0, pos);
		command = normalize(tmp_cmd);
		std::string::size_type arg_start = clean_cmd.find_first_not_of(' ', pos);
		if (arg_start != std::string::npos)
			args = clean_cmd.substr(arg_start);
	}
}

// TODO: revisar lógica
void Server::_routeCommand(Client& client, const std::string& cmd)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _routeCommand() called");

	std::string command;
	std::string args;
	_splitCommand(cmd, command, args);
	
	if (command.empty())
		return;
	if (command == "PING")
		return _command_handler.handlePing(client, args);
	if (command == "PONG")
		return;
	if (!client.isClientRegistered())
	{
		_auth_handler.handleLogin(client, cmd, _server_password);
		return;
	}
	if (command == "QUIT")
		_command_handler.handleQuit(client, args);
	else if (command == "NICK")
		_command_handler.handleNick(client, args);
	else if (command == "PASS" || command == "USER")
		client.appendOutputBuffer(":ft_irc 462 " + client.getNickname() 
			+ " :Unauthorized command (already registered)\r\n");
	else if (command == "JOIN")
		_command_handler.handleJoin(client, args);
	else if (command == "PRIVMSG")
		_command_handler.handlePrivmsg(client, args);
	else if (command == "PART")
		_command_handler.handlePart(client, args);
	else if (command == "TOPIC")
		_command_handler.handleTopic(client, args);
	else if (command == "MODE")
		_command_handler.handleMode(client, args);
	else if (command == "KICK")
		_command_handler.handleKick(client, args);
	else if (command == "INVITE")
		_command_handler.handleInvite(client, args);
	else if (command == "WHOIS")
	{
		client.appendOutputBuffer(":ft_irc 318 " + client.getNickname() 
		+ " " + args + " :End of WHOIS list\r\n");
	}
	else
	{
		client.appendOutputBuffer(":ft_irc 421 " + client.getNickname() 
			+ " " + command + " :Unknown command\r\n");
		log("Unknown command received: " + command);
	}
}

void Server::_disconnectClient(int client_fd)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _disconnectClient() called");

	std::cout << "Client <" << client_fd << "> disconnected" << std::endl;
	close(client_fd);
	_map_connected_clients.erase(client_fd);
	for (std::map<std::string, Channel>::iterator it = _map_channels.begin();
		 it != _map_channels.end(); ++it)
	{
		if (it->second.hasClient(client_fd))
		{
			it->second.removeClient(client_fd);
			if (it->second.isClientMapEmpty())
			{
				_map_channels.erase(it);
				it = _map_channels.begin();
				if (it == _map_channels.end())
					break;
			}
		}
	}
	for (std::vector<struct pollfd>::iterator poll_it = _vec_client_fds.begin(); 
		 poll_it != _vec_client_fds.end(); ++poll_it)
	{
		if (poll_it->fd == client_fd)
		{
			_vec_client_fds.erase(poll_it);
			break;
		}
	}
}
