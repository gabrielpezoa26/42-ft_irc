/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:27:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/04/01 14:26:27 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

/* ---------- Canonical Form ---------- */
Server::Server()
: _server_port(0), _server_password("default_password"),_auth_handler(_map_connected_clients)
{
	if (DEBUG_SERVER)
		printDebug("Server-> Default constructor called");
}

Server::Server(const Server& other)
: _server_port(other._server_port), _server_password(other._server_password), _auth_handler(_map_connected_clients)
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
		
		ssize_t bytes_sent = send(it->first, message.c_str(), message.length(), 0);
		if (bytes_sent > 0)
		{
			it->second.eraseOutputBuffer(bytes_sent);
		}
		else if (bytes_sent == -1)
		{
			printError("Failed to send data");
		}
	}
}
void Server::_processEvents()
{
	for (size_t i = 0; i < _vec_client_fds.size(); i++)
	{
		if (_vec_client_fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
		{
			_disconnectClient(_vec_client_fds[i].fd);
			i--; continue;
		}

		if (_vec_client_fds[i].revents & POLLIN)
		{
			if (_vec_client_fds[i].fd == _server_socket_fd)
				_handleNewConnection();
			else
			{
				if (!_handleClientActivity(_vec_client_fds[i].fd))
				{
					i--; // Compensate for vector shift
					continue;
				}
			}
		}
	
		if (i < _vec_client_fds.size() && (_vec_client_fds[i].revents & POLLOUT))
		{
			_handleClientWrite(_vec_client_fds[i].fd);
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
			throw std::runtime_error("Error on poll()");
		}
		_processEvents();
	}
	_closeFds();
}

/* ---------- Client Handlers ---------- */
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
		log("Error on accept()");
		return ;
	}
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		log("Error on fcntl()");
		return ;
	}
	_new_client_poll.fd = client_socket;
	_new_client_poll.events = POLLIN;
	_new_client_poll.revents = 0;
	_vec_client_fds.push_back(_new_client_poll);

	client.setClientFd(client_socket);
	_map_connected_clients[client_socket] = client;
}

//refatorar
bool Server::_handleClientActivity(int client_fd)
{
	char client_message[1024];
	memset(client_message, 0, sizeof(client_message));
	ssize_t bytes_received = recv(client_fd, client_message, sizeof(client_message) - 1, 0);

	if (bytes_received <= 0)
	{
		if (bytes_received != 0)
			std::cerr << "Error: connection lost on client <" << client_fd << ">." << std::endl;
		_disconnectClient(client_fd);
		return false;
	}
	std::string new_data(client_message, bytes_received);
	std::map<int, Client>::iterator it = _map_connected_clients.find(client_fd);
	if (it == _map_connected_clients.end()) return false;

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
void Server::_handlePingCommand(Client& client, const std::string& args)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _handlePingCommand() called");

	if (args.empty())
		client.appendOutputBuffer(":ft_irc 409 * :No origin specified\r\n");
	else
		client.appendOutputBuffer("PONG :" + args + "\r\n");
}

void Server::_routeCommand(Client& client, const std::string& cmd)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _routeCommand() called");

	if (cmd.empty())
		return;
	std::string command;
	std::string args;
	std::string::size_type pos = cmd.find(' ');
	if (pos == std::string::npos)
	{
		command = cmd;
		args = "";
	}
	else
	{
		command = cmd.substr(0, pos);
		std::string::size_type arg_start = cmd.find_first_not_of(' ', pos);
		if (arg_start != std::string::npos)
			args = cmd.substr(arg_start);
	}
	command = normalize(command);
	if (command == "PING")
		_handlePingCommand(client, args);
	else if (!client.isClientRegistered())
		_auth_handler.handleLogin(client, cmd, _server_password);
	else
	{
		if (command == "QUIT")
			_disconnectClient(client.getClientFd());
		else if (command == "PRIVMSG")
			_handlePrivmsg(client, args);
		else if (command == "NICK")
			_auth_handler.handleLogin(client, cmd, _server_password);
		else if (command == "JOIN" || command == "MODE" || command == "KICK" || command == "INVITE" || command == "TOPIC")
			log("Routed " + command + " to Channel class (pending implementation)");
		else
			log("Unknown command or not handled: " + command);
	}
}

void Server::_disconnectClient(int client_fd)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _disconnectClient() called");

	std::cout << "Client <" << client_fd << "> disconnected" << std::endl;
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

void Server::_handlePrivmsg(Client& client, const std::string& args)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _handlePrivmsg() called");

	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 411 " + client.getNickname() + " :No recipient given (PRIVMSG)\r\n");
		return;
	}
	std::string::size_type space_pos = args.find(' ');
	if (space_pos == std::string::npos)
	{
		client.appendOutputBuffer(":ft_irc 412 " + client.getNickname() + " :No text to send\r\n");
		return;
	}
	std::string target = args.substr(0, space_pos);
	std::string message = args.substr(space_pos + 1);
	if (!message.empty() && message[0] == ':')
		message = message.substr(1);
	std::string full_msg = ":" + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1 PRIVMSG " + target + " :" + message + "\r\n";

	if (target[0] == '#')
	{
		log("Routing PRIVMSG to channel " + target);
	}
	else
	{
		bool target_found = false;
		for (std::map<int, Client>::iterator it = _map_connected_clients.begin(); it != _map_connected_clients.end(); ++it)
		{
			if (it->second.getNickname() == target)
			{
				it->second.appendOutputBuffer(full_msg);
				target_found = true;
				break;
			}
		}
		
		if (!target_found)
		{
			client.appendOutputBuffer(":ft_irc 401 " + client.getNickname() + " " + target + " :No such nick/channel\r\n");
		}
	}
}
