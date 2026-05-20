/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:14:41 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/19 20:03:49 by gcesar-n         ###   ########.fr       */
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

bool Auth::_validatePassword(Client& client, const std::string& cmd, const std::string& server_password) const
{
	if (DEBUG_AUTH)
		printDebug("Auth-> _validatePassword() called");

	if (cmd.empty())
	{
		client.appendOutputBuffer(":ft_irc 461 * PASS :Not enough parameters\r\n");
		return false;
	}
	if (cmd != server_password)
	{
		client.appendOutputBuffer(":ft_irc 464 * :Password incorrect\r\n");
		return false;
	}
	if (DEBUG_AUTH)
		printDebug("DEBUG: password is correct ");
	client.markPasswordStatus(true);
	return true;
}

bool Auth::_validateNickname(Client& client, const std::string& cmd) const
{
	if (DEBUG_AUTH)
		printDebug("Auth-> _validateNickname() called");
	if (cmd.empty())
	{
		client.appendOutputBuffer(":ft_irc 431 " + client.getNickname() + " :No nickname given\r\n");
		return false;
	}
	std::string valid_special = "-[]\\_^{}|";
	if (isdigit(cmd[0]))
	{
		client.appendOutputBuffer(":ft_irc 432 " + client.getNickname() + " " + cmd + " :Erroneous nickname\r\n");
		return false;
	}
	for (size_t i = 0; i < cmd.length(); i++)
	{
		if (!isalnum(cmd[i]) && valid_special.find(cmd[i]) == std::string::npos)
		{
			client.appendOutputBuffer(":ft_irc 432 " + client.getNickname() + " " + cmd + " :Erroneous nickname\r\n");
			return false;
		}
	}
	std::map<int, Client>::const_iterator it;
	for (it = _existing_clients.begin(); it != _existing_clients.end(); ++it)
	{
		if (it->first != client.getClientFd() && it->second.getNickname() == cmd)
		{
			client.appendOutputBuffer(":ft_irc 433 " + client.getNickname() + " " + cmd + " :Nickname is already in use\r\n");
			return false;
		}
	}
	if (client.isClientRegistered())
	{
		std::string nick_change_msg = ":" + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1 NICK :" + cmd + "\r\n";
		client.appendOutputBuffer(nick_change_msg);
	}
	if (DEBUG_AUTH)
		printDebug("DEBUG: successfully set nickname");
	client.setNickname(cmd);
	client.markNicknameStatus(true);
	return true;
}

bool Auth::_isValidParameterAmount(const std::string& cmd) const
{
	if (DEBUG_AUTH)
		printDebug("Auth-> _isValidParameterAmount() called");
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
	if (DEBUG_AUTH)
		printDebug("Auth-> _extractInfo() called");

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
	if (!_isValidParameterAmount(cmd))
	{
		std::string err = ":ft_irc 461 " + client.getNickname() + " USER :Not enough parameters\r\n";
		client.appendOutputBuffer(err);
		return false;
	}
	if (cmd.empty())
	{
		log("USER rejected: empty string");
		return false;
	}
	if (DEBUG_AUTH)
		printDebug("DEBUG: sucessfully set username");
	if (!_isValidParameterAmount(cmd))
		return false;
	_extractInfo(client, cmd);
	client.markUsernameStatus(true);
	return true;
}

void Auth::_parseCommand(const std::string& cmd, std::string& command, std::string& args) const
{
	if (DEBUG_AUTH)
		printDebug("Auth-> _parseCommand() called");
	std::string clean_cmd = trim(cmd);
	if (clean_cmd.empty())
		return;
	std::string::size_type pos = clean_cmd.find(' ');
	if (pos == std::string::npos)
	{
		command = clean_cmd;
		args = "";
	}
	else
	{
		command = clean_cmd.substr(0, pos);
		std::string::size_type arg_start = clean_cmd.find_first_not_of(' ', pos);
		if (arg_start != std::string::npos)
		args = clean_cmd.substr(arg_start);
	}
	command = normalize(command);
}

void Auth::_sendWelcomeMessage(Client& client) const
{
	std::string welcome = ":ft_irc 001 " + client.getNickname() + 
		" :Welcome to the ft_irc network " + 
		client.getNickname() + "!" + client.getUsername() + 
		"@127.0.0.1\r\n";
	client.appendOutputBuffer(welcome);
}

void Auth::handleLogin(Client& client, const std::string& raw_line, const std::string& server_password)
{
	if (DEBUG_AUTH)
		printDebug("Auth-> handleLogin() called");

	std::string command;
	std::string args;
	_parseCommand(raw_line, command, args);
	bool was_already_registered = client.isClientRegistered();
	if (command == "CAP")
		return;
	if (command == "PASS")
	{
		_validatePassword(client, args, server_password);
	}
	else if (command == "NICK" || command == "USER")
	{
		if (!client.hasPassword())
		{
			client.appendOutputBuffer(":ft_irc 451 * :You have not registered (PASS required first)\r\n");
			return;
		}
		if (command == "NICK")
			_validateNickname(client, args);
		else
			_validateUsername(client, args);
	}
	else
	{
		client.appendOutputBuffer(":ft_irc 451 * :You have not registered\r\n");
	}
	if (!was_already_registered && client.isClientRegistered())
	{
		_sendWelcomeMessage(client);
	}
}
