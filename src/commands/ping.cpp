/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 23:29:26 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/16 16:20:30 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"

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
