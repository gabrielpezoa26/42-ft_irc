/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 23:29:12 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/08 23:39:49 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"

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
