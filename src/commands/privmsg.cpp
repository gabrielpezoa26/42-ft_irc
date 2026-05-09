/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 23:31:13 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/08 23:40:24 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"

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
