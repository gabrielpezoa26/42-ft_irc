/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 23:29:29 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/16 15:55:44 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"

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

