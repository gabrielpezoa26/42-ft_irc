/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:42:08 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/02/20 10:48:59 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

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