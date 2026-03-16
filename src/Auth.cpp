/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:14:41 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/16 15:07:30 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Auth::Auth(std::map<int, Client>& clients_map) : _existing_clients(clients_map) 
{
	if (DEBUG_AUTH)
		printDebug("Auth-> Default constructor called");
}

Auth::~Auth()
{
	if (DEBUG_AUTH)
		printDebug("Auth-> Destructor called");
}

//TODO: add códigos de erro protocolo irc
bool Auth::_validatePassword(Client& client, const std::string& cmd, const std::string& server_password)
{
	if (DEBUG_AUTH)
		printDebug("Auth-> _validatePassword() called");

	if (cmd != server_password)
	{
		log("incorrect password!");
		return false;
	}
	client.flipPass();
	printDebug("senha deu bommm");
	return true;
}
//TODO: validar nick e username
bool Auth::_validateNickname(Client& client, const std::string& cmd)
{
	if (!client.hasPass())
	{
		log("vishh 1");
		return false;
	}
	else if (cmd.empty())
	{
		log("vishhh 2");
		return false;
	}
	std::map<int, Client>::iterator it;
	for(it = _existing_clients.begin(); it != _existing_clients.end(); ++it)
	{
		if (it->second.getNickname() == cmd)
		{
			log("vishhh 3");
			return false;
		}
	}
	client.setNickname(cmd);
	client.flipNickname();
	return true;
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
	else if (command == "NICK")
		_validateNickname(client, args);
	// else if (command == "USER")
	// 	_validateUsername();
	// else
	// 	log("vishhh");
}
