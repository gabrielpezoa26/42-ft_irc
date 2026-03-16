/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:42:08 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/16 19:21:18 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/utils.hpp"

void log(std::string message)
{
	std::cout << message << std::endl;
}

void logColor(std::string message, std::string color)
{
	std::cout << color << message << RESET << std::endl;
}

void printDebug(std::string message)
{
	std::cout << PURPLE << message << RESET << std::endl;
}

void printError(std::string message)
{
	std::cout << RED << message << RESET << std::endl;
}

void printCurrentTime()
{
	std::time_t current_time = std::time(0);
	char* readable_current_time = std::ctime(&current_time);
	std::cout << GREEN << readable_current_time << RESET;
}
