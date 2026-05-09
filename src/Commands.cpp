/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 17:22:43 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/08 18:55:41 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Commands.hpp"

Commands::Commands(std::map<int, Client>& clients, std::map<std::string, Channel>& channels)
: _map_connected_clients(clients), _map_channels(channels)
{
	if (DEBUG_COMMANDS)
		printDebug("Commands-> Constructor called");
}
 
Commands::Commands(const Commands& other)
: _map_connected_clients(other._map_connected_clients),
	_map_channels(other._map_channels)
{
	if (DEBUG_COMMANDS)
		printDebug("Commands-> Copy constructor called");
}
 
Commands::~Commands()
{
	if (DEBUG_COMMANDS)
		printDebug("Commands-> Destructor called");
}






bool Commands::routeJoin(Client* client, const std::string& channel_name, const std::string& password)
{
	if (!client)
		return false;

	std::map<std::string, Channel>::iterator it = _map_channels.find(channel_name);

	if (it == _map_channels.end())
	{
		Channel new_channel(channel_name);
		_map_channels.insert(std::make_pair(channel_name, new_channel));
		it = _map_channels.find(channel_name);
	}
	return it->second.join(client, password);
}

void Commands::handleJoin(Client& client, const std::string& args)
{
	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " JOIN :Not enough parameters\r\n");
		return;
	}

	std::string channel_name = args.substr(0, args.find(' '));
	std::string password = "";
	
	size_t space_pos = args.find(' ');
	if (space_pos != std::string::npos)
		password = args.substr(space_pos + 1);

	channel_name = trim(channel_name);
	password = trim(password);
	if (channel_name.empty() || channel_name[0] != '#')
	{
		client.appendOutputBuffer(":ft_irc 403 " + client.getNickname() + " " + channel_name + " :No such channel\r\n");
		return;
	}
	if (!this->routeJoin(&client, channel_name, password))
	{
		client.appendOutputBuffer(":ft_irc 475 " + client.getNickname() 
			+ " " + channel_name + " :Cannot join channel\r\n");
	}
}





void Commands::handlePrivmsg(Client& client, const std::string& args)
{
	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 411 " + client.getNickname() + " :No recipient given (PRIVMSG)\r\n");
		return;
	}

	std::string::size_type space_pos = args.find(' ');
	if (space_pos == std::string::npos)
	{
		client.appendOutputBuffer(":ft_irc 412 " + client.getNickname() + " :No text to send\r\n");
		return;
	}
	std::string target = args.substr(0, space_pos);
	std::string message = args.substr(space_pos + 1);
	
	if (!message.empty() && message[0] == ':')
		message = message.substr(1);

	if (message.empty())
	{
		client.appendOutputBuffer(":ft_irc 412 " + client.getNickname() + " :No text to send\r\n");
		return;
	}
	target = trim(target);
	std::string full_msg = ":" + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1 PRIVMSG " + target + " :" + message + "\r\n";
	if (target[0] == '#')
	{
		std::map<std::string, Channel>::iterator it = _map_channels.find(target);
		if (it == _map_channels.end() || !it->second.hasClient(client.getClientFd()))
			client.appendOutputBuffer(":ft_irc 404 " + client.getNickname() + " " + target + " :Cannot send to channel\r\n");
		else
			it->second.broadcastExcept(client.getClientFd(), full_msg);
	}
	else
	{
		bool flag_target_found = false;
		for (std::map<int, Client>::iterator it = _map_connected_clients.begin(); it != _map_connected_clients.end(); ++it)
		{
			if (it->second.getNickname() == target)
			{
				it->second.appendOutputBuffer(full_msg);
				flag_target_found = true;
				break;
			}
		}
		if (!flag_target_found)
		{
			client.appendOutputBuffer(":ft_irc 401 " + client.getNickname() + " " + target + " :No such nick/channel\r\n");
		}
	}
}





void Commands::handleQuit(Client& client, const std::string& args)
{
	std::string reason;
	if (args.empty())
		reason = "Leaving";
	else
		reason = args;
	if (!reason.empty() && reason[0] == ':')
		reason = reason.substr(1);
	std::string error_msg = "Closing Link: " + client.getNickname() + " (Quit: " + reason + ")\r\n";
	client.appendOutputBuffer(error_msg);
	client.setQuitting(true);
}





void Commands::handleNick(Client& client, const std::string& args)
{
	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 431 " + client.getNickname() + " :No nickname given\r\n");
		return;
	}
	std::string new_nick = args;
	if (!new_nick.empty() && new_nick[0] == ':')
		new_nick = new_nick.substr(1);
	new_nick = trim(new_nick);
	if (new_nick.empty())
		return;
	for (std::map<int, Client>::iterator it = _map_connected_clients.begin(); it != _map_connected_clients.end(); ++it)
	{
		if (it->second.getNickname() == new_nick && it->first != client.getClientFd())
		{
			client.appendOutputBuffer(":ft_irc 433 " + client.getNickname() + " " + new_nick + " :Nickname is already in use\r\n");
			return;
		}
	}
	std::string old_nick = client.getNickname();
	client.setNickname(new_nick);
	client.appendOutputBuffer(":" + old_nick + "!" + client.getUsername() + "@127.0.0.1 NICK " + new_nick + "\r\n");
}




void Commands::handlePing(Client& client, const std::string& args)
{
	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 409 " + client.getNickname() + " :No origin specified\r\n");
		return;
	}

	std::string token = args;
	if (token[0] == ':')
		token = token.substr(1);

	client.appendOutputBuffer(":ft_irc PONG ft_irc :" + token + "\r\n");
}






static bool parseKickArgs(const std::string& args, std::string& channel_name, std::string& target_user, std::string& reason)
{
	size_t first_space = args.find(' ');
	if (first_space == std::string::npos)
		return false;

	channel_name = trim(args.substr(0, first_space));

	size_t second_space = args.find(' ', first_space + 1);
	if (second_space != std::string::npos)
	{
		target_user = trim(args.substr(first_space + 1, second_space - first_space - 1));
		reason = args.substr(second_space + 1);
		
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
	}
	else
	{
		target_user = trim(args.substr(first_space + 1));
		reason = "Kicked";
	}

	return true;
}

static int getTargetFd(Channel& channel, const std::string& target_user)
{
	std::map<int, Client*> chan_clients = channel.getClientsMap();
	for (std::map<int, Client*>::iterator cit = chan_clients.begin(); cit != chan_clients.end(); ++cit)
	{
		if (cit->second->getNickname() == target_user)
			return cit->first;
	}
	return -1;
}

void Commands::handleKick(Client& client, const std::string& args)
{
	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " KICK :Not enough parameters\r\n");
		return;
	}

	std::string channel_name, target_user, reason;
	if (!parseKickArgs(args, channel_name, target_user, reason))
	{
		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " KICK :Not enough parameters\r\n");
		return;
	}
	std::map<std::string, Channel>::iterator it = _map_channels.find(channel_name);
	if (it == _map_channels.end())
	{
		client.appendOutputBuffer(":ft_irc 403 " + client.getNickname() + " " + channel_name + " :No such channel\r\n");
		return;
	}

	Channel& channel = it->second;
	if (!channel.hasClient(client.getClientFd()))
	{
		client.appendOutputBuffer(":ft_irc 442 " + client.getNickname() + " " + channel_name + " :You're not on that channel\r\n");
		return;
	}
	if (!channel.isOperator(client.getClientFd()))
	{
		client.appendOutputBuffer(":ft_irc 482 " + client.getNickname() + " " + channel_name + " :You're not channel operator\r\n");
		return;
	}
	int target_fd = getTargetFd(channel, target_user);
	if (target_fd == -1)
	{
		client.appendOutputBuffer(":ft_irc 441 " + client.getNickname() + " " + target_user + " " + channel_name + " :They aren't on that channel\r\n");
		return;
	}

	std::string kick_msg = ":" + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1 KICK " + channel_name + " " + target_user + " :" + reason + "\r\n";
	channel.broadcast(kick_msg);
	channel.removeClient(target_fd);

	if (channel.isClientMapEmpty())
	{
		_map_channels.erase(it);
	}
}





void Commands::handlePart(Client& client, const std::string& args)
{
	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " PART :Not enough parameters\r\n");
		return;
	}

	std::string channel_name = trim(args.substr(0, args.find(' ')));
	std::map<std::string, Channel>::iterator it = _map_channels.find(channel_name);
	if (it == _map_channels.end())
	{
		client.appendOutputBuffer(":ft_irc 403 " + client.getNickname() + " " + channel_name + " :No such channel\r\n");
		return;
	}
	Channel& channel = it->second;
	if (!channel.hasClient(client.getClientFd()))
	{
		client.appendOutputBuffer(":ft_irc 442 " + client.getNickname() + " " + channel_name + " :You're not on that channel\r\n");
		return;
	}
	std::string part_msg = ":" + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1 PART " + channel_name + "\r\n";
	channel.broadcast(part_msg);
	channel.removeClient(client.getClientFd());
	if (channel.isClientMapEmpty())
		_map_channels.erase(it);
}










// TODO: revisar funcionalidade & refatorar
static void _applyModeK(Channel& channel, bool add_mode, const std::string& mode_args)
{
	if (add_mode && !mode_args.empty())
		channel.setPassword(mode_args);
	else if (!add_mode)
		channel.setPassword("");
}

static void _applyModeO(Client& client, Channel& channel, bool add_mode, const std::string& mode_args)
{
	if (mode_args.empty())
		return;
	int target_fd = getTargetFd(channel, mode_args);
	if (target_fd == -1)
	{
		client.appendOutputBuffer(":ft_irc 441 " + client.getNickname() + " " + mode_args + " " + channel.getChannelName() + " :They aren't on that channel\r\n");
		return;
	}
	if (add_mode)
		channel.setOperator(target_fd);
	else
		channel.removeOperator(target_fd);
}

static void _applyModeL(Channel& channel, bool add_mode, const std::string& mode_args)
{
	if (add_mode && !mode_args.empty())
		channel.setUserLimit(atoi(mode_args.c_str()));
	else if (!add_mode)
		channel.setUserLimit(-1);
}

static void _applyModes(Client& client, Channel& channel, const std::string& mode_string, const std::string& mode_args)
{
	bool add_mode = true;
	for (size_t i = 0; i < mode_string.length(); i++)
	{
		char mode = mode_string[i];
		if (mode == '+')
		{
			add_mode = true;
			continue;
		}
		if (mode == '-')
		{
			add_mode = false;
			continue;
		}
		if (mode == 'i')
			channel.setInviteOnly(add_mode);
		else if (mode == 't')
			channel.setTopicRestricted(add_mode);
		else if (mode == 'k')
			_applyModeK(channel, add_mode, mode_args);
		else if (mode == 'o')
			_applyModeO(client, channel, add_mode, mode_args);
		else if (mode == 'l')
			_applyModeL(channel, add_mode, mode_args);
	}
}

static void _parseModeArgs(const std::string& args, std::string& target, std::string& mode_string, std::string& mode_args)
{
	size_t space_pos = args.find(' ');
	target = trim(args.substr(0, space_pos));
	std::string rest = "";
	if (space_pos != std::string::npos)
		rest = args.substr(space_pos + 1);
	size_t arg_pos = rest.find(' ');
	if (arg_pos != std::string::npos)
	{
		mode_string = rest.substr(0, arg_pos);
		mode_args = trim(rest.substr(arg_pos + 1));
	}
	else
	{
		mode_string = rest;
	}
}

static void _queryChannelModes(Client& client, Channel& channel)
{
	std::string modes = "+";
	if (channel.getInviteOnly())
		modes += "i";
	if (channel.getTopicRestricted())
		modes += "t";
	if (!channel.getPassword().empty())
		modes += "k";
	if (channel.getUserLimit() > 0)
		modes += "l";
	client.appendOutputBuffer(":ft_irc 324 " + client.getNickname() + " " + channel.getChannelName() + " " + modes + "\r\n");
}

void Commands::handleMode(Client& client, const std::string& args)
{
	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n");
		return;
	}
	std::string target;
	std::string mode_string;
	std::string mode_args;
	_parseModeArgs(args, target, mode_string, mode_args);

	if (target == client.getNickname())
	{
		client.appendOutputBuffer(":ft_irc 221 " + client.getNickname() + " +i\r\n");
		return;
	}
	if (target[0] != '#')
	{
		client.appendOutputBuffer(":ft_irc 502 " + client.getNickname() + " :Cannot change mode for other users\r\n");
		return;
	}
	std::map<std::string, Channel>::iterator it = _map_channels.find(target);
	if (it == _map_channels.end())
	{
		client.appendOutputBuffer(":ft_irc 403 " + client.getNickname() + " " + target + " :No such channel\r\n");
		return;
	}
	Channel& channel = it->second;
	if (mode_string.empty())
	{
		_queryChannelModes(client, channel);
		return;
	}
	if (!channel.isOperator(client.getClientFd()))
	{
		client.appendOutputBuffer(":ft_irc 482 " + client.getNickname() + " " + target + " :You're not channel operator\r\n");
		return;
	}
	_applyModes(client, channel, mode_string, mode_args);
	std::string mode_msg = ":" + client.getNickname() + "!" + client.getUsername()
		+ "@127.0.0.1 MODE " + target + " " + mode_string;
	if (!mode_args.empty())
		mode_msg += " " + mode_args;
	mode_msg += "\r\n";
	channel.broadcast(mode_msg);
}
