/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:42:42 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/03/17 10:42:50 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <ctime>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define BLUE "\033[34m"
#define PURPLE "\033[35m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

void log(std::string message);
void logColor(std::string message, std::string color);
void printDebug(std::string message);
void printError(std::string message);
void printCurrentTime();
std::string normalize(std::string& cmd);

template <typename T>
void debugVar(const std::string name, const T value)
{
	std::cout << YELLOW << "DEBUG: " << name << " = " << value << RESET << std::endl;
}

template <typename T>
void printObject(const T& obj)
{
	typename T::const_iterator start = obj.begin();
	for (; start != obj.end(); ++start)
		std::cout << " " << *start;
	std::cout << std::endl;
}

#endif