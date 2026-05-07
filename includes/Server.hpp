/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:24:23 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/07 18:10:02 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <stdlib.h>
#include <limits>
#include <exception>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <poll.h>
#include <csignal>
#include <vector>
#include <map>
#include "Client.hpp"
#include "Auth.hpp"
#include "utils.hpp"
#include "Channel.hpp"
#include "Commands.hpp"

#define DEBUG_SERVER false

class Server
{
	private:
		int _server_port;
		std::string _server_password;
		static bool _continue_running;
		int _server_socket_fd;
		struct sockaddr_in _server_address;
		struct sockaddr_in _client_address;

		struct pollfd _new_client_poll;
		std::vector<struct pollfd> _vec_client_fds;
		std::map<int, Client> _map_connected_clients;
		std::map<std::string, Channel> _map_channels;
		
		Auth _auth_handler;
		Commands _command_handler;
		bool _isValidPort(const std::string &port);
		bool _isValidPassword(const std::string &password);
		// std::map<std::string, Channel> _channels;


		static void _handleSignals(int signum);
		void setupSignals();
		void _handleNewConnection();
		// void _handleNickCommand(Client& client, const std::string& args);
		bool _handleClientActivity(int client_fd);
		void _disconnectClient(int client_fd);
		void _splitCommand(const std::string& cmd, std::string& command, std::string& args);
		void _handlePingCommand(Client& client, const std::string& args);
		void _handleModeCommand(Client& client, const std::string& args);
		void _routeCommand(Client& client, const std::string& cmd);
		// void _handleQuitCommand(std::string& args, Client& client);
		// void _handleJoinCommand(Client& client, const std::string& args);


		void _prepareEvents();
		void _handleClientWrite(int client_fd);
		// void _handlePrivmsg(Client& client, const std::string& args);
		void _processEvents();

		void _closeFds();


	public:
		Server();
		Server(const Server& other);
		~Server();
		Server& operator=(const Server& other);

		void init(char **argv);
		void setSocket();
		void run();
};

#endif
