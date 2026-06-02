/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 23:29:12 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/06/02 19:30:41 by gcesar-n         ###   ########.fr       */
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
	std::cout << "DEBUG handleJoin args='" << args << "'" << std::endl;

	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " JOIN :Not enough parameters\r\n");
		return;
	}

	size_t comma_pos = args.find(',');
	std::string clean_args = (comma_pos != std::string::npos) ? args.substr(0, comma_pos) : args;

	std::string channel_name = clean_args.substr(0, clean_args.find(' '));
	std::string password = "";

	size_t space_pos = clean_args.find(' ');
	if (space_pos != std::string::npos)
		password = clean_args.substr(space_pos + 1);

	channel_name = trim(channel_name);
	password = trim(password);

	if (!password.empty() && password[0] == '#')
		password = "";

	if (channel_name.empty() || channel_name[0] != '#')
	{
		client.appendOutputBuffer(":ft_irc 403 " + client.getNickname() + " " + channel_name + " :No such channel\r\n");
		return;
	}

	if (!this->routeJoin(&client, channel_name, password))
	{
		std::map<std::string, Channel>::iterator it = _map_channels.find(channel_name);
		if (it != _map_channels.end() && it->second.hasClient(client.getClientFd()))
			return;
		if (it != _map_channels.end() && it->second.getInviteOnly())
			client.appendOutputBuffer(":ft_irc 473 " + client.getNickname()
				+ " " + channel_name + " :Cannot join channel (+i)\r\n");
		else if (it != _map_channels.end() && !it->second.getPassword().empty())
			client.appendOutputBuffer(":ft_irc 475 " + client.getNickname()
				+ " " + channel_name + " :Cannot join channel (+k)\r\n");
		else if (it != _map_channels.end() && it->second.getUserLimit() > 0
			&& (int)it->second.getClientCount() >= it->second.getUserLimit())
			client.appendOutputBuffer(":ft_irc 471 " + client.getNickname()
				+ " " + channel_name + " :Cannot join channel (+l)\r\n");
		else
			client.appendOutputBuffer(":ft_irc 475 " + client.getNickname()
				+ " " + channel_name + " :Cannot join channel\r\n");
	}
}

// void Commands::handleJoin(Client& client, const std::string& args)
// {
// 	std::cout << "DEBUG handleJoin args='" << args << "'" << std::endl;
	
// 	if (args.empty())
// 	{
// 		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " JOIN :Not enough parameters\r\n");
// 		return;
// 	}

// 	size_t delim = args.find_first_of(" ,");
// 	std::string channel_name = args.substr(0, delim);
// 	std::string password = "";

// 	if (delim != std::string::npos && args[delim] == ' ')
// 	{
// 		std::string rest = trim(args.substr(delim + 1));
// 		if (!rest.empty() && rest[0] != '#')
// 			password = rest;
// 	}

// 	channel_name = trim(channel_name);

// 	if (channel_name.empty() || channel_name[0] != '#')
// 	{
// 		client.appendOutputBuffer(":ft_irc 403 " + client.getNickname() + " " + channel_name + " :No such channel\r\n");
// 		return;
// 	}

// 	if (!this->routeJoin(&client, channel_name, password))
// 	{
// 		std::map<std::string, Channel>::iterator it = _map_channels.find(channel_name);
// 		if (it != _map_channels.end() && it->second.hasClient(client.getClientFd()))
// 			return;
// 		if (it != _map_channels.end() && it->second.getInviteOnly())
// 			client.appendOutputBuffer(":ft_irc 473 " + client.getNickname()
// 				+ " " + channel_name + " :Cannot join channel (+i)\r\n");
// 		else if (it != _map_channels.end() && !it->second.getPassword().empty())
// 			client.appendOutputBuffer(":ft_irc 475 " + client.getNickname()
// 				+ " " + channel_name + " :Cannot join channel (+k)\r\n");
// 		else if (it != _map_channels.end() && it->second.getUserLimit() > 0
// 			&& (int)it->second.getClientCount() >= it->second.getUserLimit())
// 			client.appendOutputBuffer(":ft_irc 471 " + client.getNickname()
// 				+ " " + channel_name + " :Cannot join channel (+l)\r\n");
// 		else
// 			client.appendOutputBuffer(":ft_irc 475 " + client.getNickname()
// 				+ " " + channel_name + " :Cannot join channel\r\n");
// 	}
// }
