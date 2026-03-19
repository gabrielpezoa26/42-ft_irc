/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:14:41 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/19 17:29:01 by gcesar-n         ###   ########.fr       */
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
	logColor("DEBUG: password is correct ", GREEN);
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
	logColor("DEBUG: sucessfully set nickname", GREEN);
	client.setNickname(cmd);
	client.markNicknameStatus(true);
	return true;
}

//TODO: add códigos de erro protocolo irc
//TODO: trocar msgs de erro
bool Auth::_isValidParameterAmount(const std::string& cmd) const
{
	std::string::size_type pos = 0;
	int parameter_count = 0;
	while (pos < cmd.length() && parameter_count < 4)
	{
		pos = cmd.find_first_not_of(' ', pos);
		if (pos == std::string::npos)
			break;
		if (cmd[pos] == ':')
		{
			parameter_count++;
			break;
		}
		pos = cmd.find(' ', pos);
		parameter_count++;
	}
	if (parameter_count < 4)
	{
		log("USER rejected: not enough parameters");
		return false;
	}
	return true;
}

void Auth::_extractInfo(Client& client, const std::string& cmd) const
{
	std::string extracted_username = cmd.substr(0, cmd.find_first_of(' '));
	std::string extracted_realname = "";
	std::string::size_type colon_pos = cmd.find(':');
	if (colon_pos != std::string::npos)
	{
		extracted_realname = cmd.substr(colon_pos + 1);
	}
	else
	{
		std::string::size_type start = 0;
		for (int i = 0; i < 3; i++)
		{
			start = cmd.find_first_not_of(' ', start);
			start = cmd.find(' ', start);
		}
		start = cmd.find_first_not_of(' ', start);
		if (start != std::string::npos)
			extracted_realname = cmd.substr(start);
	}
	client.setUsername(extracted_username);
	client.setRealName(extracted_realname);
}

bool Auth::_validateUsername(Client& client, const std::string& cmd) const
{
	if (DEBUG_AUTH)
		printDebug("Auth-> _validateUsername() called");

	if (client.hasUsername())
	{
		log("USER rejected: already registered");
		return false;
	}
	if (!client.hasPassword())
	{
		log("USER rejected: must insert password first");
		return false;
	}
	if (cmd.empty())
	{
		log("USER rejected: empty string");
		return false;
	}
	logColor("DEBUG: sucessfully set username", GREEN);
	if (!_isValidParameterAmount(cmd))
		return false;
	_extractInfo(client, cmd);
	client.markUsernameStatus(true);
	return true;
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
	command = normalize(command);
	if (command == "PASS")
		_validatePassword(client, args, server_password);
	else if (command == "NICK")
		_validateNickname(client, args);
	else if (command == "USER")
		_validateUsername(client, args);
	else
		printError("nao eh comando");
	if (client.isClientRegistered())
	{
		std::string welcome_msg = ":001 " + client.getNickname() + 
			" :Welcome to ft_irc " + client.getNickname() + "! " + client.getUsername() + "@127.0.0.1\r\n";
		
		client.appendOutputBuffer(welcome_msg);
	}
}
