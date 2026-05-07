/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 17:23:42 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/07 20:43:02 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/Channel.hpp"

/* ---------- Canonical Form ---------- */
Channel::Channel()
: _channel_name("default_name"),
_channel_topic(""),
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
_channel_topic(""),
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
_channel_operators(other._channel_operators),
_invited_clients(other._invited_clients),
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
	_map_connect_clients.clear();
	_channel_operators.clear();
	_invited_clients.clear();
}

Channel& Channel::operator=(const Channel& other)
{
	if (DEBUG_CHANNEL)
		printDebug("Channel-> Assign operator called");
	
	if (this != &other)
	{
		_channel_name = other._channel_name;
		_channel_topic = other._channel_topic;
		_map_connect_clients = other._map_connect_clients;
		_channel_operators = other._channel_operators;
		_invited_clients = other._invited_clients;
		_channel_password = other._channel_password;
		_is_invite_only = other._is_invite_only;
		_is_topic_restricted = other._is_topic_restricted;
		_user_limit = other._user_limit;
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
	_channel_operators.erase(client_fd);
	_invited_clients.erase(client_fd);
	if (DEBUG_CHANNEL)
		std::cout << "Channel-> Client removed (fd=" << client_fd << ")" << std::endl;
}

bool Channel::hasClient(int client_fd) const
{
	return _map_connect_clients.find(client_fd) != _map_connect_clients.end();
}

/* ---------- Broadcasting ---------- */
void Channel::broadcast(const std::string& message)
{
	for (std::map<int, Client*>::iterator it = _map_connect_clients.begin();
		 it != _map_connect_clients.end(); ++it)
	{
		if (it->second)
			it->second->appendOutputBuffer(message);
	}
}

void Channel::broadcastExcept(int sender_fd, const std::string& message)
{
	for (std::map<int, Client*>::iterator it = _map_connect_clients.begin();
		 it != _map_connect_clients.end(); ++it)
	{
		if (it->first != sender_fd && it->second)
			it->second->appendOutputBuffer(message);
	}
}

void Channel::setOperator(int client_fd)
{
	_channel_operators.insert(client_fd);
}

bool Channel::isOperator(int client_fd) const
{
	return _channel_operators.find(client_fd) != _channel_operators.end();
}

void Channel::removeOperator(int client_fd)
{
	_channel_operators.erase(client_fd);
}

void Channel::inviteClient(int client_fd)
{
	_invited_clients.insert(client_fd);
}

bool Channel::isInvited(int client_fd) const
{
	return _invited_clients.find(client_fd) != _invited_clients.end();
}

std::string Channel::getTopic() const
{
	return _channel_topic;
}

void Channel::setTopic(const std::string& topic)
{
	_channel_topic = topic;
}

bool Channel::canChangeTopic(int client_fd) const
{
	if (_is_topic_restricted && !this->isOperator(client_fd))
		return false;
	return true;
}

void Channel::setPassword(const std::string& password)
{
	_channel_password = password;
}

std::string Channel::getPassword() const
{
	return _channel_password;
}

void Channel::setInviteOnly(bool flag)
{
	_is_invite_only = flag;
}

bool Channel::getInviteOnly() const
{
	return _is_invite_only;
}

void Channel::setTopicRestricted(bool flag)
{
	_is_topic_restricted = flag;
}

bool Channel::getTopicRestricted() const
{
	return _is_topic_restricted;
}

void Channel::setUserLimit(int limit)
{
	_user_limit = limit;
}

int Channel::getUserLimit() const
{
	return _user_limit;
}

std::string Channel::getChannelName() const
{
	return _channel_name;
}

size_t Channel::getClientCount() const
{
	return _map_connect_clients.size();
}

bool Channel::isEmpty() const
{
	return _map_connect_clients.empty();
}

std::map<int, Client*> Channel::getClients() const
{
	return _map_connect_clients;
}

std::vector<std::string> Channel::getClientNicknames() const
{
	std::vector<std::string> nicks;
	for (std::map<int, Client*>::const_iterator it = _map_connect_clients.begin();
		 it != _map_connect_clients.end(); ++it)
	{
		if (it->second)
			nicks.push_back(it->second->getNickname());
	}
	return nicks;
}

bool Channel::canJoin(Client* client, const std::string& password) const
{
	if (!client)
		return false;

	if (this->hasClient(client->getClientFd()))
		return false;
	if (!_channel_password.empty() && _channel_password != password)
		return false;
	if (_is_invite_only && !this->isInvited(client->getClientFd()))
		return false;
	if (_user_limit > 0 && (int)_map_connect_clients.size() >= _user_limit)
		return false;
	return true;
}

bool Channel::join(Client* client, const std::string& password)
{
	if (!client)
		return false;
	if (!this->canJoin(client, password))
		return false;
	this->addClient(client);
	if (_map_connect_clients.size() == 1)
		this->setOperator(client->getClientFd());
	std::string join_msg = ":" + client->getNickname() + "!" + client->getUsername()
		+ "@127.0.0.1 JOIN " + _channel_name + "\r\n";
	this->broadcast(join_msg);
	std::string topic_msg = ":ft_irc 332 " + client->getNickname() 
		+ " " + _channel_name + " :" + _channel_topic + "\r\n";
	client->appendOutputBuffer(topic_msg);
	std::string names_msg = ":ft_irc 353 " + client->getNickname() + " = " 
		+ _channel_name + " :";
	for (std::map<int, Client*>::iterator it = _map_connect_clients.begin();
		 it != _map_connect_clients.end(); ++it)
	{
		if (this->isOperator(it->first))
			names_msg += "@";
		names_msg += it->second->getNickname();
		std::map<int, Client*>::iterator next = it;
		++next;
		if (next != _map_connect_clients.end())
			names_msg += " ";
	}
	names_msg += "\r\n";
	client->appendOutputBuffer(names_msg);
	std::string eol_msg = ":ft_irc 366 " + client->getNickname() 
		+ " " + _channel_name + " :End of NAMES list\r\n";
	client->appendOutputBuffer(eol_msg);

	return true;
}
