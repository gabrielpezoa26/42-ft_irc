/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcesar-n <gcesar-n@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 22:28:58 by gcesar-n          #+#    #+#             */
/*   Updated: 2026/06/08 22:35:11 by gcesar-n         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_HPP
#define SIGNALS_HPP

#include <unistd.h>
#include "Server.hpp"

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