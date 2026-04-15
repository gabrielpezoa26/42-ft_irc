/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 17:23:42 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/04/15 18:34:52 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

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

Channel& Channel::operator=(const Channel& other)
{
	if (DEBUG_CHANNEL)
		printDebug("Channel-> Assignment operator called");
	if (this != &other)
	{
		_channel_name = other._channel_name;
		_channel_topic = other._channel_topic;
		_is_invite_only = other._is_invite_only;
		_is_topic_restricted = other._is_topic_restricted;
		_user_limit = other._user_limit;
		_channel_password = other._channel_password;
		_map_connect_clients = other._map_connect_clients;
	}
	return *this;
}
