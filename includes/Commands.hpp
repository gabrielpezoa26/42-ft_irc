/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 17:22:54 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/08 18:37:55 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <map>
#include <string>
#include <stdlib.h>
#include "Client.hpp"
#include "Channel.hpp"

#define DEBUG_COMMANDS false

class Commands
{
	private:
		std::map<int, Client>&		_map_connected_clients;
		std::map<std::string, Channel>&	_map_channels;
 
		std::string		_trim(const std::string& str) const;
 
	public:
		Commands(std::map<int, Client>& clients, std::map<std::string, Channel>& channels);
		Commands(const Commands& other);
		~Commands();
		Commands& operator=(const Commands& other);
 
		bool routeJoin(Client* client, const std::string& channel_name, const std::string& password);

		void handleJoin(Client& client, const std::string& args);
		void handlePrivmsg(Client& client, const std::string& args);
		void handleQuit(Client& client, const std::string& args);
		void handleNick(Client& client, const std::string& args);
		void handlePing(Client& client, const std::string& args);
		void handleMode(Client& client, const std::string& args);
		void handleKick(Client& client, const std::string& args);
		void handlePart(Client& client, const std::string& args);



};

#endif