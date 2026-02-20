/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:24:23 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/20 17:25:07 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <stdlib.h>
#include <limits>
#include <exception>
#include "utils.hpp"

#define DEBUG false

class Server
{
	private:
		std::string _defined_port;
		std::string _defined_password;


	public:
		Server();
		Server(const Server& other);  //cpa n vai precisar
		~Server();
		Server& operator=(const Server& other); //cpa n vai precisar

		void init(char **argv);
		bool isValidPort(const std::string &port);
		bool isPasswordValid(const std::string &password);
	};


// Your executable will be run as follows:
// 	./ircserv <port> <password>

#endif