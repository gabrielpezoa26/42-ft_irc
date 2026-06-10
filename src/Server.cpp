/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:27:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/06/09 23:13:20 by gcesar-n         ###   ########.fr       */
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
bool Server::_parsePort(const std::string &input_port)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _parsePort() called");

	std::string temp = input_port;
	std::string valid_chars = "0123456789";
	if (temp.find_first_not_of(valid_chars) != std::string::npos)
		return false;
	if (!atoi(temp.c_str()) || atoi(temp.c_str()) < 0 || atol(temp.c_str()) > std::numeric_limits<int>::max())
		return false;
	_server_port = atoi(temp.c_str());
	if (_server_port < 1024 || _server_port > 65535)  // reserved and ceiling ports
		return false;
	return true;
}

bool Server::_parsePassword(const std::string &input_password)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _parsePassword() called");

	_server_password = input_password;
	if (_server_password.empty())
		return false;
	for (size_t i = 0; i < _server_password.length(); i++)
	{
		if (!isascii(_server_password[i]))
			return false;
	}
	return true;
}

/* ---------- Server Methods ---------- */
void Server::init(char **argv)
{
	if (DEBUG_SERVER)
		printDebug("Server-> init() called");

	std::string input_port = argv[1];
	std::string input_password = argv[2];

	if (!_parsePort(input_port) || !_parsePassword(input_password))
	{
		throw std::invalid_argument("Server-> Exception caught: invalid parameters");
	}
}

void Server::setSocket()
{
	if (DEBUG_SERVER)
		printDebug("Server-> setSocket() called");

	_server_address.sin_family = AF_INET;  // IPv4
	_server_address.sin_port = htons(_server_port);  // which port to listen
	_server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);  // creates the endpoint
	_server_address.sin_addr.s_addr = INADDR_ANY;  // bind to all available network interfaces
	int flag = 1;

	if (_server_socket_fd == -1)
		throw std::runtime_error("Error while socket creation");
	if (setsockopt(_server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) //allow the port to be reused fast)
		throw std::runtime_error("Error while setting SO_REUSEADDR on main socket");
	if (fcntl(_server_socket_fd, F_SETFL, O_NONBLOCK) == -1) // sets to non blocking I/O
		throw std::runtime_error("Error while setting O_NONBLOCK on main socket");
	if (bind(_server_socket_fd, (struct sockaddr *)&_server_address, sizeof(_server_address)) == -1) //links the socket fd with the ip address
		throw std::runtime_error("Error while binding socket");
	if (listen(_server_socket_fd, SOMAXCONN) == -1) //sets the socket to wait connection
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
	SignalHandler::setupSignals();
	while (SignalHandler::isRunning())
	{
		_prepareEvents();
		int poll_result = poll(&_vec_client_fds[0], _vec_client_fds.size(), -1);
		if (poll_result == -1 && SignalHandler::isRunning() == true) 
			throw std::runtime_error("Error while polling");
		_processEvents();
	}
	_shutdownServer();
}

void Server::_prepareEvents()
{
	if (DEBUG_SERVER)
		printDebug("Server-> _prepareEvents() called");
	for (size_t i = 0; i < _vec_client_fds.size(); i++)
	{
		if (_vec_client_fds[i].fd == _server_socket_fd)
			continue;
		
		// associate integer fd with client object
		std::map<int, Client>::iterator it = _map_connected_clients.find(_vec_client_fds[i].fd);
		if (it != _map_connected_clients.end())
		{
			if (!it->second.getOutputBuffer().empty())
				_vec_client_fds[i].events = POLLIN | POLLOUT;  //set for sending and receive data
			else
				_vec_client_fds[i].events = POLLIN;  //set for only receiving data
		}
	}
}

void Server::_processEvents()
{
	if (DEBUG_SERVER)
		printDebug("Server-> _processEvents() called");

	for (size_t index = 0; index < _vec_client_fds.size(); index++)
	{
		// check if socket connection was dropped, error, or invalid
		if (_vec_client_fds[index].revents & (POLLHUP | POLLERR | POLLNVAL))
		{
			_disconnectClient(_vec_client_fds[index].fd);
			index--;
			continue;
		}
		// detects incoming data on socket
		if (_vec_client_fds[index].revents & POLLIN)
		{
			if (_vec_client_fds[index].fd == _server_socket_fd)
				_handleNewConnection();
			else
			{
				// processes regular activity 
				if (!_handleClientActivity(_vec_client_fds[index].fd))
				{
					index--;
					continue;
				}
			}
		}
		//confirms vector size and socket state
		if (index < _vec_client_fds.size() && (_vec_client_fds[index].revents & POLLOUT))
		{
			_handleClientWrite(_vec_client_fds[index].fd);

			//checks state post write
			std::map<int, Client>::iterator it = _map_connected_clients.find(_vec_client_fds[index].fd);
			if (it != _map_connected_clients.end() && it->second.getIsQuitting() && it->second.getOutputBuffer().empty())
			{
				_disconnectClient(_vec_client_fds[index].fd);
				index--;
				continue;
			}
		}
	}
}

void Server::_handleClientWrite(int client_fd)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _handleClientWrite() called");

	// searches for the specific client
	std::map<int, Client>::iterator it = _map_connected_clients.find(client_fd);

	if (it != _map_connected_clients.end())
	{
		// retrieves the pending data that needs to be sent to this specific client
		const std::string& message = it->second.getOutputBuffer();
		if (LOG_CONSOLE)
			std::cout << PURPLE << "DEBUG WRITE [fd=" << client_fd << "] buffer='" << message << "'" << RESET <<std::endl;
		
		// sends the message through the socket
		ssize_t bytes_sent = send(it->first, message.c_str(), message.length(), 0);

		if (LOG_CONSOLE)
			std::cout << PURPLE << "DEBUG WRITE [fd=" << client_fd << "] bytes_sent=" << bytes_sent << RESET << std::endl;
		
		// confirms data was sent with success
		if (bytes_sent > 0)
			it->second.eraseOutputBuffer(bytes_sent);
		else if (bytes_sent == -1)
			printError("Failed to send data");
	}
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
	_new_client_poll.revents = 0;  // reset
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

	if (LOG_CONSOLE)
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
	// client graceful disconnect or network error
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

	// process all complete commands
	it->second.appendInputBuffer(new_data);
	while (true)
	{
		it = _map_connected_clients.find(client_fd);
		if (it == _map_connected_clients.end())  //safety check
			return false;

		std::string extracted_cmd = it->second.fetchCommand();
		if (extracted_cmd.empty())
			break;
		_routeCommand(it->second, extracted_cmd);
	}
	return true;
}

void Server::_routeCommand(Client& client, const std::string& input_cmd)
{
	if (DEBUG_SERVER)
		printDebug("Server-> _routeCommand() called");

	std::string command;
	std::string args;
	splitCommand(input_cmd, command, args);
	
	if (command.empty())
		return;
	if (command == "PING")
		return _command_handler.handlePing(client, args);
	if (command == "PONG")
		return;
	if (!client.isClientRegistered())
	{
		_auth_handler.handleLogin(client, input_cmd, _server_password);
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
	else if (command == "WHO")
	{
		client.appendOutputBuffer(":ft_irc 315 " + client.getNickname() + " " + args + " :End of WHO list\r\n");
	}
	else if (command == "WHOIS")
	{
		client.appendOutputBuffer(":ft_irc 318 " + client.getNickname() + " " + args + " :End of WHOIS list\r\n");
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

	std::map<int, Client>::iterator client_iterator = _map_connected_clients.find(client_fd);
	std::string nick = "unknown";
	if (client_iterator != _map_connected_clients.end())
		nick = client_iterator->second.getNickname();
	std::cout << "Client disconnected: fd=" << client_fd << " nick=" << nick << std::endl;
	close(client_fd);

	_map_connected_clients.erase(client_fd);
	for (std::map<std::string, Channel>::iterator it = _map_channels.begin(); it != _map_channels.end();)
	{
		if (it->second.hasClient(client_fd))
		{
			it->second.removeClient(client_fd);
			if (it->second.isClientMapEmpty())
			{
				_map_channels.erase(it++);  // delete channel if empty
				continue;
			}
			it->second.promoteNextOperator();
		}
		++it;
	}
	for (std::vector<struct pollfd>::iterator poll_it = _vec_client_fds.begin(); poll_it != _vec_client_fds.end(); ++poll_it)
	{
		if (poll_it->fd == client_fd)
		{
			_vec_client_fds.erase(poll_it);  //removes the socket from poll list
			break;
		}
	}
}

void Server::_shutdownServer()
{
	if (DEBUG_SERVER)
		printDebug("Server-> _shutdownServer() called");

	for (std::map<int, Client>::iterator it = _map_connected_clients.begin(); it != _map_connected_clients.end(); ++it)
	{
		if (it->first != -1)
		{
			std::string message = "ERROR :Server shutting down\r\n";
			send(it->first, message.c_str(), message.size(), 0);
			close(it->first);
		}
	}
	_map_connected_clients.clear();
	_vec_client_fds.clear();
	if (_server_socket_fd != -1)
		close(_server_socket_fd);
}
