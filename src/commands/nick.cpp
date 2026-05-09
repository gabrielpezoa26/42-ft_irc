/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 23:29:20 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/08 23:40:07 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"

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
