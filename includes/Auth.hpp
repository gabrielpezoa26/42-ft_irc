/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 08:00:17 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/16 18:17:24 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTH_HPP
#define AUTH_HPP

#include <iostream>
#include <string>
#include <map>
#include <cctype>
#include "Client.hpp"

#define DEBUG_AUTH true

class Auth
{
	private:
		bool _validatePassword(Client& client, const std::string& cmd, const std::string& server_password) const;
		bool _validateNickname(Client& client, const std::string& cmd) const;
		bool _validateUsername(Client& client, const std::string& cmd) const;
		std::map<int, Client>& _existing_clients;
		std::string _normalize(std::string& cmd);
		



	public:
		//TODO: terminar forma canonica
		Auth(std::map<int, Client>& clients_map);
		~Auth();

		void handleLogin(Client &client, const std::string &cmd, const std::string& server_password);

};

#endif