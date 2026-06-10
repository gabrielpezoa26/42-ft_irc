/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 22:15:14 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/06/09 23:27:28 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/SignalHandler.hpp"

SignalHandler::SignalHandler() {}
SignalHandler::SignalHandler(const SignalHandler& other) { (void)other; }
SignalHandler& SignalHandler::operator=(const SignalHandler& other) { (void)other; return *this; }
SignalHandler::~SignalHandler() {}

bool SignalHandler::_continue_running = true;

void SignalHandler::handleSignals(int signum)
{
	(void)signum;
	SignalHandler::_continue_running = false;
}

void SignalHandler::setupSignals()
{
	struct sigaction sa;
	
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SignalHandler::handleSignals;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

bool SignalHandler::isRunning()
{
	return SignalHandler::_continue_running;
}
