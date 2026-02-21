/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:24:23 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/21 13:55:54 by gcesar-n         ###   ########.fr       */
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

#define DEBUG true

class Server
{
	private:
		int _defined_port;
		std::string _defined_password;
		int server_fd_socket;
		struct sockaddr_in add;

		bool _isValidPort(const std::string &port);
		bool _isValidPassword(const std::string &password);

	public:
		Server();
		Server(const Server& other);  //cpa n vai precisar
		~Server();
		Server& operator=(const Server& other); //cpa n vai precisar

		void init(char **argv);
		void setSocket();
		template <typename T>

		// apagar dps
		void printVarDebug(const std::string name, const T value)
		{
			std::cout << "DEBUG: " << name << " = " << value << std::endl;
		}
};


// Your executable will be run as follows:
// 	./ircserv <port> <password>

#endif