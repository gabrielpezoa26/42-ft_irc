/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:40:00 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/20 18:41:18 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// implementar sinais
static void server_loop(char **argv)
{
	try
	{
		Server server;
		server.init(argv);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

int main(int argc, char **argv)
{
	if (argc != 3)
		return 1;

	server_loop(argv);
	return 0;
}
