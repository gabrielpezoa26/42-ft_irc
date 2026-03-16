/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:14:41 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/16 13:12:53 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Auth::Auth() { }

Auth::~Auth() { }

//TODO: validar senha, nick e username
bool Auth::_validatePassword(Client& client, const std::string& cmd, const std::string& server_password)
{
	if (cmd != server_password)
	{
		log("incorrect password!");
		return false;
	}
	client.flipPass();
	return true;
}

bool Auth::_validateNickname(Client& client, const std::string& cmd)
{
	
}

// bool Auth::_validateUsername(Client& client, const std::string& cmd)
// {

// }

void Auth::handleLogin(Client& client, const std::string& cmd, const std::string& server_password)
{
	if (DEBUG_AUTH)
		printDebug("Auth-> handleLogin() called");
	
	std::string command;
	std::string args;
	std::string::size_type pos = cmd.find(' ');
	if (pos == std::string::npos)
	{
		command = cmd;
		args = "";
	}
	else
	{
		command = cmd.substr(0, pos);
		std::string::size_type arg_start = cmd.find_first_not_of(' ', pos);
		if (arg_start == std::string::npos)
			args = "";
		else
			args = cmd.substr(arg_start);
	}
	if (command == "PASS")
		_validatePassword(client, args, server_password);
	// else if (command == "NICK")
	// 	_validateNickname();
	// else if (command == "USER")
	// 	_validateUsername();
	// else
	// 	log("vishhh");
}
