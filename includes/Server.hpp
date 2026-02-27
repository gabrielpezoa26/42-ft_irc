/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:24:23 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/27 00:02:07 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <stdlib.h>
#include <limits>
#include <exception>
#include "utils.hpp"
#include <sys/socket.h> //socket()
#include <netinet/in.h> //sockaddr_in struct
#include <arpa/inet.h> //htons()
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <poll.h>
#include <csignal>
#include <vector>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define BLUE "\033[34m"
#define PURPLE "\033[35m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

#define DEBUG true

class Server
{
	private:
		int _server_port;
		std::string _server_password;
		static bool _is_running;

		int _server_socket_fd;
		struct sockaddr_in _server_adress;
		struct pollfd _new_client;
		std::vector<struct pollfd> _vec_client_fds;

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
		
		template <typename T>
		void debugVar(const std::string name, const T value)
		{
			std::cout << YELLOW << "DEBUG: " << name << " = " << value << RESET << std::endl;
		}
};

#endif