/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:20:56 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/11 16:37:44 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include "utils.hpp"

#define DEBUG_CLIENT false

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


	public:
		Client();
		Client(const Client& other);
		~Client();
		Client& operator=(const Client& other);

		std::string fetchCommand();
		void appendInputBuffer(const std::string &in_to_append);
		void appendOutputBuffer(const std::string &out_to_append);

		std::string getNickname();
		std::string getUsername();
		int getClientFd();

		void setNickname(std::string given_nickname);
		void setUsername(std::string given_username);
		void setClientFd(int given_fd);

		void flipNickname();
		void flipUsername();
		void flipPass();
		void flipAll();  //cpa so vamo usar esse
		bool isClientRegistered();

};

#endif
