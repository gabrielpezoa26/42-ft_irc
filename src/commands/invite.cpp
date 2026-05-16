/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 18:07:04 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/16 17:33:07 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Commands.hpp"
#include <sys/socket.h>

void Commands::handleInvite(Client& client, const std::string& args)
{
          std::cout << "\n--- STARTING INVITE DIAGNOSTIC ---" << std::endl;
          std::cout << "Raw args: [" << args << "]" << std::endl;

          if (args.empty())
          {
                    std::cout << "Failed: args empty" << std::endl;
                    client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " INVITE :Not enough parameters\r\n");
                    return;
          }
 
          size_t space_pos = args.find(' ');
          if (space_pos == std::string::npos)
          {
                    std::cout << "Failed: no space found in args" << std::endl;
                    client.appendOutputBuffer(":ft_irc 461 " + client.getNickname() + " INVITE :Not enough parameters\r\n");
                    return;
          }
 
          std::string target_nick = trim(args.substr(0, space_pos));
          std::string channel_name = trim(args.substr(space_pos + 1));
          std::cout << "Parsed Target: [" << target_nick << "], Channel: [" << channel_name << "]" << std::endl;
 
          std::map<std::string, Channel>::iterator it = _map_channels.find(channel_name);
          if (it == _map_channels.end())
          {
                    std::cout << "Failed: channel not found in _map_channels" << std::endl;
                    client.appendOutputBuffer(":ft_irc 403 " + client.getNickname() + " " + channel_name + " :No such channel\r\n");
                    return;
          }
          Channel& channel = it->second;
          std::cout << "Channel found successfully." << std::endl;
 
          if (channel.getClientsMap().find(client.getClientFd()) == channel.getClientsMap().end())
          {
                    std::cout << "Failed: operator (sender) is not inside the channel" << std::endl;
                    client.appendOutputBuffer(":ft_irc 442 " + client.getNickname() + " " + channel_name + " :You're not on that channel\r\n");
                    return;
          }
 
          if (channel.getInviteOnly() && !channel.isOperator(client.getClientFd()))
          {
                    std::cout << "Failed: sender is not a channel operator" << std::endl;
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
                    std::cout << "Failed: target nickname not found in active clients" << std::endl;
                    client.appendOutputBuffer(":ft_irc 401 " + client.getNickname() + " " + target_nick + " :No such nick\r\n");
                    return;
          }
          std::cout << "Target FD found: " << target_fd << std::endl;
 
          Client& target = _map_connected_clients[target_fd];

          if (channel.getClientsMap().find(target.getClientFd()) != channel.getClientsMap().end())
          {
                    std::cout << "Failed: target is already in the channel" << std::endl;
                    client.appendOutputBuffer(":ft_irc 443 " + client.getNickname() + " " + target_nick + " " + channel_name + " :is already on channel\r\n");
                    return;
          }
 
          std::cout << "All checks passed! Adding to invite list and appending to buffers." << std::endl;
          channel.inviteClient(target.getClientFd());
 
          client.appendOutputBuffer(":ft_irc 341 " + client.getNickname() + " " + target_nick + " " + channel_name + "\r\n");
          target.appendOutputBuffer(":" + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1 INVITE " + target_nick + " :" + channel_name + "\r\n");
          
          std::cout << "--- END INVITE DIAGNOSTIC ---\n" << std::endl;
}
