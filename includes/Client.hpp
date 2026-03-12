/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:20:56 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/12 12:04:04 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include "utils.hpp"

#define DEBUG_CLIENT true

class Client
{
	private:
		std::string _client_username;
		std::string _client_nickname;
		int _client_fd;
		bool _has_nickname;
		bool _has_username;
		bool _has_pass;
		std::string _input_buffer;
		std::string _output_buffer;

		void flipNickname();
		void flipUsername();
		void flipPass();
		void flipAll();  //cpa n vai usar esse
		bool isClientRegistered();

		void _authClient();


	public:
		Client();
		Client(const Client& other);
		~Client();
		Client& operator=(const Client& other);

		std::string fetchCommand();
		void appendInputBuffer(const std::string &in_to_append);
		void appendOutputBuffer(const std::string &out_to_append);

		void setNickname(std::string given_nickname);
		void setUsername(std::string given_username);
		void setClientFd(int given_fd);
		std::string getNickname();
		std::string getUsername();
		int getClientFd();
};

#endif
