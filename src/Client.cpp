/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:20:55 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/16 19:17:19 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

/* ---------- Canonical Form ---------- */
Client::Client()
 : _client_username("default_username"),
_client_nickname("default_nickname"),
_has_set_password(false),
_has_set_nickname(false),
_has_set_username(false)
{
	if (DEBUG_CLIENT)
		printDebug("Client-> Default constructor called");
}

Client::Client(const Client& other)
 : _client_username(other._client_username),
_client_nickname(other._client_nickname),
_client_fd(other._client_fd),
_has_set_password(other._has_set_password),
_has_set_nickname(other._has_set_nickname),
_has_set_username(other._has_set_username)
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
		_client_username = other._client_username;
		_client_nickname = other._client_nickname;
		_client_fd = other._client_fd;
		_has_set_nickname = other._has_set_nickname;
		_has_set_username = other._has_set_username;
		_has_set_password = other._has_set_password;
	}
	return *this;
}



/* ---------- Methods ---------- */
std::string Client::fetchCommand()
{
	if (DEBUG_CLIENT)
		printDebug("Client-> fetchCommand() called");

	std::string::size_type pos;
	std::string result;
	pos = _input_buffer.find("\r\n");
	if (pos == std::string::npos)
	{
		result = "";
	}
	else
	{
		result = _input_buffer.substr(0, pos);
		_input_buffer.erase(0, pos + 2); //fica +2 por causa do '\r\n'
	}
	return result;
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



/* ---------- Getters && Setters ---------- */
std::string Client::getNickname() const { return _client_nickname; }
std::string Client::getUsername() const { return _client_username; }
int Client::getClientFd() const { return _client_fd; }

void Client::setNickname(std::string given_nickname) { _client_nickname = given_nickname; }
void Client::setUsername(std::string given_username) { _client_username = given_username; }
void Client::setClientFd(int given_fd) { _client_fd = given_fd; }

bool Client::hasPassword() const { return _has_set_password; }
bool Client::hasNickname() const { return _has_set_nickname; }
bool Client::hasUsername() const { return _has_set_username; }



/* ---------- Authentication---------- */
void Client::markPasswordStatus(bool value)
{
	if (DEBUG_CLIENT)
		printDebug("Client-> markPasswordStatus() called");

	_has_set_password = value;
}

void Client::markNicknameStatus(bool value)
{
	if (DEBUG_CLIENT)
		printDebug("Client-> markNicknameStatus() called");

	_has_set_nickname = value;
}

void Client::markUsernameStatus(bool value)
{
	if (DEBUG_CLIENT)
		printDebug("Client-> markUsernameStatus() called");

	_has_set_username = value;
}

bool Client::isClientRegistered() const
{
	if (DEBUG_CLIENT)
		printDebug("Client-> isClientRegistered() called");

	if (!_has_set_password || !_has_set_nickname || !_has_set_username)
		return false;
	return true;
}
