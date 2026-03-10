/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:20:55 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/10 17:21:35 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

/* ---------- Canonical Form ---------- */
Client::Client()
 : _username("default_username"),
_nickname("default_nickname"),
_has_nickname(false),
_has_username(false),
_has_pass(false)
{
	if (DEBUG_CLIENT)
		printDebug("Client-> Default constructor called");
}

Client::Client(const Client& other)
 : _username(other._username),
_nickname(other._nickname),
_client_fd(other._client_fd),
_has_nickname(other._has_nickname),
_has_username(other._has_username),
_has_pass(other._has_pass)
{
	if (DEBUG_CLIENT)
		printDebug("Client-> Copy constructor called");
	
}

Client::~Client()
{
	if (DEBUG_CLIENT)
		printDebug("Client-> Destructor called");
	
}

Client& Client::operator=(const Client& other)
{
	if (DEBUG_CLIENT)
		printDebug("Client-> assign operator called");

	if (this != &other)
	{
		_username = other._username;
		_nickname = other._nickname;
		_client_fd = other._client_fd;
		_has_nickname = other._has_nickname;
		_has_username = other._has_username;
		_has_pass = other._has_pass;
	}
	return *this;
}



/* ---------- Methods ---------- */
std::string Client::fetchCommand()
{
	if (DEBUG_CLIENT)
		printDebug("Client-> fetchCommand() called");

	std::string::size_type pos;
	std::string res;

	pos = _input_buffer.find("\r\n");
	if (pos == std::string::npos)
	{
		res = "";
	}
	else
	{
		res = _input_buffer.substr(0, pos);
		debugVar("res", res);
		_input_buffer.erase(0, pos + 2); //fica +2 por causa do '\r\n'
	}
	return res;
}

void Client::appendInputBuffer(const std::string &in_to_append)
{
	if (DEBUG_CLIENT)
		printDebug("Client-> appendInputBuffer() called");

	_input_buffer.append(in_to_append);
}

void Client::appendOutputBuffer(const std::string &out_to_append)
{
	if (DEBUG_CLIENT)
		printDebug("Client-> appendOutputBuffer() called");	

	_output_buffer.append(out_to_append);
}
