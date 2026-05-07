/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 17:19:43 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/07 20:36:14 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include "Client.hpp"

#define DEBUG_CHANNEL false

class Channel
{
	private:
		std::string _channel_name;
		std::string _channel_topic;
		std::map<int, Client*> _map_connect_clients;
		std::set<int> _channel_operators;
		std::set<int> _invited_clients;


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

		bool join(Client* client, const std::string& password);
		bool canJoin(Client* client, const std::string& password) const;

		void broadcast(const std::string& message);
		void broadcastExcept(int sender_fd, const std::string& message);

		// +o
		void setOperator(int client_fd);
		bool isOperator(int client_fd) const;
		void removeOperator(int client_fd);

		// +i
		void inviteClient(int client_fd);
		bool isInvited(int client_fd) const;

		// +t
		std::string getTopic() const;
		void setTopic(const std::string& topic);
		bool canChangeTopic(int client_fd) const;

		/* Mode setters/getters */
		void setPassword(const std::string& password);
		std::string getPassword() const;
		void setInviteOnly(bool flag);
		bool getInviteOnly() const;
		void setTopicRestricted(bool flag);
		bool getTopicRestricted() const;
		void setUserLimit(int limit);
		int getUserLimit() const;

		std::string getChannelName() const;
		size_t getClientCount() const;
		bool isEmpty() const;
		std::map<int, Client*> getClients() const;
		std::vector<std::string> getClientNicknames() const;
};


#endif