/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:20:55 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/05 09:48:05 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client()
 : _username("default_username") , _nickname("default_nickname"), _is_authenticated(false)
{
	if (DEBUG_CLIENT)
		printDebug("Client-> Default constructor called");
}

Client::Client(const Client& other)
 : _username(other._username),
_nickname(other._nickname),
_client_fd(other._client_fd),
_is_authenticated(other._is_authenticated)
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
		printDebug("Client-> assignment operator called");

	if (this != &other)
	{
		_username = other._username;
		_nickname = other._nickname;
		_client_fd = other._client_fd;
		_is_authenticated = other._is_authenticated;
	}
	return *this;
	
}
