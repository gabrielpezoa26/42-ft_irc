/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:20:56 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/26 18:30:21 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include "utils.hpp"

#define DEBUG true

class Client
{
	private:
		std::string _user_name;
		std::string _nick_name;
		int _fd;
		bool _is_authenticated;

	public:
		Client();
		Client(const Client& other);
		~Client();

		Client& operator=(const Client& other);
};



#endif