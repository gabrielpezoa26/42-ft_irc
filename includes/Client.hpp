/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:20:56 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/05 09:49:36 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include "utils.hpp"

#define DEBUG_CLIENT true

class Client
{
	private:
		std::string _username;
		std::string _nickname;
		int _client_fd;
		bool _is_authenticated;

	public:
		Client();
		Client(const Client& other);
		~Client();

		Client& operator=(const Client& other);
};



#endif