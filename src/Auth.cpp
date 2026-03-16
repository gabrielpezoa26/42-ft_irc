/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:14:41 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/16 08:40:05 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Auth::Auth() { }

Auth::~Auth() { }

// bool Auth::_validatePassword(Client& client, const std::string& cmd, const std::string& server_password);
// bool Auth::_validateNickname(Client& client, const std::string& cmd);
// bool Auth::_validateUsername(Client& client, const std::string& cmd);

void Auth::handleLogin(Client& client, const std::string& cmd, const std::string& server_password)
{
	if (DEBUG_AUTH)
		printDebug("Auth-> handleLogin() called");
	
	(void)client;
	(void)cmd;
	(void)server_password;
	// if (cmd == "PASS")
	// 	_validatePassword();
	// else if (cmd == "NICK")
	// 	_validateNickname();
	// else if (cmd == "USER")
	// 	_validateUsername();
	// else
	// 	log("vishhh");
}
