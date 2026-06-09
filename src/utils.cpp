/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:42:08 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/06/08 22:44:43 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utils.hpp"

void log(std::string message) { std::cout << message << std::endl; }

void logColor(std::string message, std::string color) { std::cout << color << message << RESET << std::endl; }

void printDebug(std::string message) { std::cout << PURPLE << message << RESET << std::endl; }

void printError(std::string message) { std::cout << RED << message << RESET << std::endl; }

void printCurrentTime()
{
	std::time_t current_time = std::time(0);
	char* readable_current_time = std::ctime(&current_time);
	std::cout << GREEN << readable_current_time << RESET;
}

std::string normalize(std::string& str)
{
	for(size_t i = 0; i < str.length(); i++)
		str[i] = std::toupper((unsigned char)str[i]);
	return str;
}

std::string trim(const std::string& str)
{
	std::string result = str;
	std::string::size_type pos = result.find_last_not_of(" \t\r\n");
	if (pos != std::string::npos)
		result.erase(pos + 1);
	else
		result.clear();
	pos = result.find_first_not_of(" \t\r\n");
	if (pos != std::string::npos)
		result.erase(0, pos);
	return result;
}

void splitCommand(const std::string& cmd, std::string& command, std::string& args)
{
	std::string clean_cmd = trim(cmd);
	if (clean_cmd.empty())
		return;

	std::string::size_type pos = clean_cmd.find(' ');
	if (pos == std::string::npos)
	{
		command = normalize(clean_cmd);
		args = "";
	}
	else
	{
		std::string tmp_cmd = clean_cmd.substr(0, pos);
		command = normalize(tmp_cmd);
		std::string::size_type arg_start = clean_cmd.find_first_not_of(' ', pos);
		if (arg_start != std::string::npos)
			args = clean_cmd.substr(arg_start);
	}
}
