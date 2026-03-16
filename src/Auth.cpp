/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:14:41 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/16 19:26:59 by gcesar-n         ###   ########.fr       */
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
//TODO: trocar msgs de erro
bool Auth::_validatePassword(Client& client, const std::string& cmd, const std::string& server_password) const
{
	if (DEBUG_AUTH)
		printDebug("Auth-> _validatePassword() called");

	if (cmd.empty())
	{
		printError("empty password");
		return false;
	}
	if (cmd != server_password)
	{
		printError("incorrect password!");
		return false;
	}
	client.markPasswordStatus(true);
	return true;
}

//TODO: add códigos de erro protocolo irc
//TODO: trocar msgs de erro
bool Auth::_validateNickname(Client& client, const std::string& cmd) const
{
	if (DEBUG_AUTH)
		printDebug("Auth-> _validateNickname() called");

	if (!client.hasPassword())
	{
		printError("Error 1");
		return false;
	}
	else if (cmd.empty())
	{
		printError("Error 2");
		return false;
	}
	std::map<int, Client>::iterator it;
	for(it = _existing_clients.begin(); it != _existing_clients.end(); ++it)
	{
		if (it->second.getNickname() == cmd)
		{
			printError("Error 3");
			return false;
		}
	}
	client.setNickname(cmd);
	client.markNicknameStatus(true);
	return true;
}

//ta incompleto
bool Auth::_validateUsername(Client& client, const std::string& cmd) const
{
	if (DEBUG_AUTH)
		printDebug("Auth-> _validateUsername() called");

	if (!client.hasPassword())
	{
		printError("Error 4");
		return false;
	}
	else if (cmd.empty())
	{
		printError("Error 5");
		return false;
	}
	//fzr o parse do username
	// ...
	client.setUsername(cmd);
	client.markUsernameStatus(true);
	return true;
}

std::string Auth::_normalize(std::string& cmd)
{
	for(size_t i = 0; i < cmd.length(); i++)
		cmd[i] = std::toupper((unsigned char)cmd[i]);
	return cmd;
}

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
	command = _normalize(command);
	if (command == "PASS")
		_validatePassword(client, args, server_password);
	else if (command == "NICK")
		_validateNickname(client, args);
	else if (command == "USER")
		_validateUsername(client, args);
	else
		printError("nao eh comando");
}
