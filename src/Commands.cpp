/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 17:22:43 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/05/08 23:50:20 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Commands.hpp"

Commands::Commands(std::map<int, Client>& clients, std::map<std::string, Channel>& channels)
: _map_connected_clients(clients), _map_channels(channels)
{
	if (DEBUG_COMMANDS)
		printDebug("Commands-> Constructor called");
}
 
Commands::Commands(const Commands& other)
: _map_connected_clients(other._map_connected_clients),
	_map_channels(other._map_channels)
{
	if (DEBUG_COMMANDS)
		printDebug("Commands-> Copy constructor called");
}

Commands::~Commands()
{
	if (DEBUG_COMMANDS)
		printDebug("Commands-> Destructor called");
}
