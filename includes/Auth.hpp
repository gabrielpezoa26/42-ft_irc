/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 08:00:17 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/25 22:15:14 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTH_HPP
#define AUTH_HPP

#include <iostream>
#include <string>
#include <map>
#include "Client.hpp"
#include "utils.hpp"

#define DEBUG_AUTH false

class Auth
{
	private:
		std::map<int, Client>& _map_existing_clients;

		bool _validatePassword(Client& client, const std::string& cmd, const std::string& server_password) const;
		bool _validateNickname(Client& client, const std::string& cmd) const;
		bool _validateUsername(Client& client, const std::string& cmd) const;

		bool _isValidParameterAmount(const std::string& cmd) const;
		void _extractInfo(Client& client, const std::string& cmd) const;
		void _parseCommand(const std::string& cmd, std::string& command, std::string& args) const;
		void _sendWelcomeMessage(Client& client) const;


	public:
		Auth(std::map<int, Client>& clients_map);
		~Auth();

		void handleLogin(Client &client, const std::string &cmd, const std::string& server_password);
};

#endif