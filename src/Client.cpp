/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 18:20:55 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/26 18:31:20 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client()
{
	if (DEBUG)
		printDebug("Client-> Default constructor called");
}

Client::Client(const Client& other)
{
	if (DEBUG)
		printDebug("Client-> Copy constructor called");
	
}

Client::~Client()
{
	if (DEBUG)
		printDebug("Client-> Destructor called");
	
}

Client& Client::operator=(const Client& other)
{
	if (DEBUG)
		printDebug("Client-> assignment operator called");
	
}
