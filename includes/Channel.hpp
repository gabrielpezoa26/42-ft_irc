/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 17:19:43 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/04/15 19:32:26 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <map>
#include "Client.hpp"

#define DEBUG_CHANNEL true

class Channel
{
	private:
		std::string _channel_name;
		std::string _channel_topic;
		std::map<int, Client*> _map_connect_clients;

		std::string _channel_password;  // +k
		bool _is_invite_only;  // +i
		bool _is_topic_restricted;  // +t
		int _user_limit;  // +l


	public:
		Channel();
		Channel(std::string given_name);
		Channel(const Channel& other);
		~Channel();
		Channel& operator=(const Channel& other);

		void addClient(Client* client);
		void removeClient(int client_fd);
		bool hasClient(int client_fd) const;
};

#endif