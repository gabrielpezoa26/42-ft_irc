/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 18:07:04 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/16 18:05:55 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"

void Commands::handleInvite(Client& client, const std::string& args)
{
	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " INVITE :Not enough parameters\r\n");
		return;
	}
 
	size_t space_pos = args.find(' ');
	if (space_pos == std::string::npos)
	{
		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " INVITE :Not enough parameters\r\n");
		return;
	}
 
	std::string target_nick = trim(args.substr(0, space_pos));
	std::string channel_name = trim(args.substr(space_pos + 1));
 
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
 
	if (channel.getInviteOnly() && !channel.isOperator(client.getClientFd()))
	{
		client.appendOutputBuffer(":ft_irc 482 " + client.getNickname() + " " + channel_name + " :You're not channel operator\r\n");
		return;
	}
 
	int target_fd = -1;
	for (std::map<int, Client>::iterator mit = _map_connected_clients.begin(); mit != _map_connected_clients.end(); ++mit)
	{
		if (mit->second.getNickname() == target_nick)
		{
			target_fd = mit->first;
			break;
		}
	}

	if (target_fd == -1)
	{
		client.appendOutputBuffer(":ft_irc 401 " + client.getNickname() + " " + target_nick + " :No such nick\r\n");
		return;
	}
 
	Client& target = _map_connected_clients[target_fd];

	if (channel.hasClient(target.getClientFd()))
	{
		client.appendOutputBuffer(":ft_irc 443 " + client.getNickname() + " " + target_nick + " " + channel_name + " :is already on channel\r\n");
		return;
	}
 
	channel.inviteClient(target.getClientFd());
 
	client.appendOutputBuffer(":ft_irc 341 " + client.getNickname() + " " + target_nick + " " + channel_name + "\r\n");
	target.appendOutputBuffer(":" + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1 INVITE " + target_nick + " :" + channel_name + "\r\n");
}
