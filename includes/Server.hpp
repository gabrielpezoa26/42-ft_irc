/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:24:23 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/22 16:12:11 by gcesar-n         ###   ########.fr       */
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
#include <ctime>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <poll.h>

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
		int _defined_port;
		std::string _defined_password;
		bool _continue;

		int _server_socket;
		struct sockaddr_in _add;
		struct pollfd _new_client;

		bool _isValidPort(const std::string &port);
		bool _isValidPassword(const std::string &password);
		void _printCurrentTime();


	public:
		Server();
		Server(const Server& other); //cpa n vai precisar
		~Server();
		Server& operator=(const Server& other); //cpa n vai precisar

		void init(char **argv);
		void setSocket();
		void run();
		
		template <typename T>
		void debugVar(const std::string name, const T value)
		{
			std::cout << YELLOW << "DEBUG: " << name << " = " << value << RESET << std::endl;
		}
};

#endif