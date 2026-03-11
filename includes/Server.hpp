/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:24:23 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/11 11:03:35 by gcesar-n         ###   ########.fr       */
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
#include "Client.hpp"
#include "utils.hpp"

#define DEBUG_SERVER true

class Server
{
	private:
		int _server_port;
		std::string _server_password;
		static bool _continue_running;
		int _server_socket_fd;
		struct sockaddr_in _server_adress;
		struct sockaddr_in _client_address;

		struct pollfd _new_client_poll;
		std::vector<struct pollfd> _vec_client_fds;
		std::vector<Client> _vec_connected_clients;  //cpa trocar pra std::set


		bool _isValidPort(const std::string &port);
		bool _isValidPassword(const std::string &password);
		void _handleNewConnection();
		void _handleClientActivity(int client_fd);



	public:
		Server();
		Server(const Server& other); //cpa n vai precisar
		~Server();
		Server& operator=(const Server& other); //cpa n vai precisar

		void init(char **argv);
		void setSocket();
		void run();

		static void handleSignals(int signum);
		void setupSignals();
};

#endif