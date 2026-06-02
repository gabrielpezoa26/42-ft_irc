/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 23:34:11 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/06/02 19:00:37 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"

static bool _isValidModeChar(char x)
{
	if (x == 'i' || x == 't' || x == 'k' || x == 'o' || x == 'l')
		return true;
	else
		return false;
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

static bool _applyModeK(Client& client, Channel& channel, bool add_mode, const std::string& mode_args)
{
	if (add_mode)
	{
		if (mode_args.empty())
		{
			client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n");
			return false;
		}
		channel.setPassword(mode_args);
		return true;
	}
	else
	{
		channel.setPassword("");
		return true;
	}
}

static bool _applyModeO(Client& client, Channel& channel, bool add_mode, const std::string& mode_args)
{
	if (mode_args.empty())
	{
		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n");
		return false;
	}
	int target_fd = getTargetFd(channel, mode_args);
	if (target_fd == -1)
	{
		client.appendOutputBuffer(":ft_irc 441 " + client.getNickname() + " " + mode_args + " " + channel.getChannelName() + " :They aren't on that channel\r\n");
		return false;
	}
	if (add_mode)
		channel.setOperator(target_fd);
	else
		channel.removeOperator(target_fd);
	return true;
}

static bool _applyModeL(Client& client, Channel& channel, bool add_mode, const std::string& mode_args)
{
	if (add_mode)
	{
		if (mode_args.empty())
		{
			client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n");
			return false;
		}
		for (size_t i = 0; i < mode_args.length(); i++)
		{
			if (!isdigit(mode_args[i]))
				return false;
		}
		int limit = atoi(mode_args.c_str());
		if (limit > 0)
		{
			channel.setUserLimit(limit);
			return true;
		}
		return false;
	}
	else
	{
		channel.setUserLimit(-1);
		return true;
	}
}

static bool _applyModes(Client& client, Channel& channel, const std::string& mode_string, const std::string& mode_args)
{
	bool add_mode = true;
	bool state_changed = false;
	
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
		if (!_isValidModeChar(mode))
			continue;
			
		if (mode == 'i')
		{
			channel.setInviteOnly(add_mode);
			state_changed = true;
		}
		else if (mode == 't')
		{
			channel.setTopicRestricted(add_mode);
			state_changed = true;
		}
		else if (mode == 'k')
		{
			if (_applyModeK(client, channel, add_mode, mode_args))
				state_changed = true;
		}
		else if (mode == 'o')
		{
			if (_applyModeO(client, channel, add_mode, mode_args))
				state_changed = true;
		}
		else if (mode == 'l')
		{
			if (_applyModeL(client, channel, add_mode, mode_args))
				state_changed = true;
		}
	}
	return state_changed;
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
	
	if (_applyModes(client, channel, mode_string, mode_args))
	{
		std::string mode_msg = ":" + client.getNickname() + "!" + client.getUsername()
			+ "@127.0.0.1 MODE " + target + " " + mode_string;
		if (!mode_args.empty())
			mode_msg += " " + mode_args;
		mode_msg += "\r\n";
		channel.broadcast(mode_msg);
	}
}
