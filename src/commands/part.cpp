/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 23:29:23 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/11 18:52:29 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"

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
