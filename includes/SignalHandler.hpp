/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 22:28:58 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/06/10 23:05:22 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include <signal.h>
#include <string.h>

class SignalHandler
{
	private:
		static bool _continue_running;

		SignalHandler();
		SignalHandler(const SignalHandler& other);
		SignalHandler& operator=(const SignalHandler& other);
		~SignalHandler();


	public:
		static void handleSignals(int signum);
		static void setupSignals();
		static bool isRunning();
};

#endif