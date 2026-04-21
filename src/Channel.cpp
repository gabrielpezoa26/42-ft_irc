/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 17:23:42 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/04/20 22:16:49 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

/* ---------- Canonical Form ---------- */
Channel::Channel()
: _channel_name("default_name"),
_channel_topic("default_topic"),
_channel_password(""),
_is_invite_only(false),
_is_topic_restricted(false),
_user_limit(-1)
{
	if (DEBUG_CHANNEL)
		printDebug("Channel-> Default constructor called");
}

Channel::Channel(std::string given_name)
: _channel_name(given_name),
_channel_topic("default_topic"),
_channel_password(""),
_is_invite_only(false),
_is_topic_restricted(false),
_user_limit(-1)
{
	if (DEBUG_CHANNEL)
		printDebug("Channel-> Parameter constructor called");
	
}

Channel::Channel(const Channel& other)
: _channel_name(other._channel_name),
_channel_topic(other._channel_topic),
_map_connect_clients(other._map_connect_clients),
_channel_password(other._channel_password),
_is_invite_only(other._is_invite_only),
_is_topic_restricted(other._is_topic_restricted),
_user_limit(other._user_limit)
{
	if (DEBUG_CHANNEL)
		printDebug("Channel-> Copy constructor called");
	
}

Channel::~Channel()
{
	if (DEBUG_CHANNEL)
		printDebug("Channel-> Destructor called");
	
}



/* ---------- Methods ---------- */
Channel& Channel::operator=(const Channel& other)
{
	if (DEBUG_CHANNEL)
		printDebug("Channel-> Assignment operator called");
	if (this != &other)
	{
		_channel_name = other._channel_name;
		_channel_topic = other._channel_topic;
		_map_connect_clients = other._map_connect_clients;
		_is_invite_only = other._is_invite_only;
		_is_topic_restricted = other._is_topic_restricted;
		_user_limit = other._user_limit;
		_channel_password = other._channel_password;
	}
	return *this;
}

void Channel::addClient(Client* client)
{
	if (client != NULL)
	{
		_map_connect_clients[client->getClientFd()] = client;
	}
}

void Channel::removeClient(int client_fd)
{
	_map_connect_clients.erase(client_fd);
}

bool Channel::hasClient(int client_fd) const
{
	return _map_connect_clients.find(client_fd) != _map_connect_clients.end();
}