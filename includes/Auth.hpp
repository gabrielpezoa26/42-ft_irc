/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 08:00:17 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/16 08:37:27 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTH_HPP
#define AUTH_HPP

#include <iostream>
#include <string>
#include "Client.hpp"

#define DEBUG_AUTH true

class Auth
{
	private:
		bool _validatePassword(Client& client, const std::string& cmd, const std::string& server_password);
		bool _validateNickname(Client& client, const std::string& cmd);
		bool _validateUsername(Client& client, const std::string& cmd);



	public:
		Auth();
		~Auth();

		void handleLogin(Client &client, const std::string &cmd, const std::string& server_password);

};

#endif