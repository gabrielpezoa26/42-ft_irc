/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 23:29:15 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/22 21:40:19 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"

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
		return;
	}
	channel.promoteNextOperator();
}
