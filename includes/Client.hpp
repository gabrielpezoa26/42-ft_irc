/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:20:56 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/16 15:50:14 by gcesar-n         ###   ########.fr       */
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
		std::string _input_buffer;
		std::string _output_buffer;

		bool _has_set_password;
		bool _has_set_nickname;
		bool _has_set_username;



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
		std::string getNickname() const;
		std::string getUsername() const;
		int getClientFd() const;

		void markPasswordStatus(bool value);
		void markNicknameStatus(bool value);
		void markUsernameStatus(bool value);
		bool isClientRegistered() const;
		
		bool hasPassword() const;
		bool hasNickname() const;
		bool hasUsername() const;

};

#endif
