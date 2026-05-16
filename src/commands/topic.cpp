/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 17:45:58 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/16 15:55:22 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"
 
static void _parseTopicArgs(const std::string& args, std::string& channel_name, std::string& topic)
{
	size_t space_pos = args.find(' ');
	channel_name = trim(args.substr(0, space_pos));
	if (space_pos == std::string::npos)
		return;
	std::string rest = args.substr(space_pos + 1);
	if (!rest.empty() && rest[0] == ':')
		rest = rest.substr(1);
	topic = trim(rest);
}
 
static void _replyTopic(Client& client, Channel& channel)
{
	const std::string& chan_name = channel.getChannelName();
	if (channel.getTopic().empty())
	{
		client.appendOutputBuffer(":ft_irc 331 " + client.getNickname()
			+ " " + chan_name + " :No topic is set\r\n");
		return;
	}
	client.appendOutputBuffer(":ft_irc 332 " + client.getNickname()
		+ " " + chan_name + " :" + channel.getTopic() + "\r\n");
}
 
void Commands::handleTopic(Client& client, const std::string& args)
{
	if (args.empty())
	{
		client.appendOutputBuffer(":ft_irc 461 " + client.getNickname()
			+ " TOPIC :Not enough parameters\r\n");
		return;
	}
 
	std::string channel_name;
	std::string topic;
	_parseTopicArgs(args, channel_name, topic);
 
	std::map<std::string, Channel>::iterator it = _map_channels.find(channel_name);
	if (it == _map_channels.end())
	{
		client.appendOutputBuffer(":ft_irc 403 " + client.getNickname()
			+ " " + channel_name + " :No such channel\r\n");
		return;
	}
	Channel& channel = it->second;
 
	if (channel.getClientsMap().find(client.getClientFd()) == channel.getClientsMap().end())
	{
		client.appendOutputBuffer(":ft_irc 442 " + client.getNickname()
			+ " " + channel_name + " :You're not on that channel\r\n");
		return;
	}
 
	if (args.find(' ') == std::string::npos)
	{
		_replyTopic(client, channel);
		return;
	}
 
	if (channel.getTopicRestricted() && !channel.isOperator(client.getClientFd()))
	{
		client.appendOutputBuffer(":ft_irc 482 " + client.getNickname()
			+ " " + channel_name + " :You're not channel operator\r\n");
		return;
	}
 
	channel.setTopic(topic);
 
	std::string topic_msg = ":" + client.getNickname() + "!" + client.getUsername()
		+ "@127.0.0.1 TOPIC " + channel_name + " :" + topic + "\r\n";
	channel.broadcast(topic_msg);
}
