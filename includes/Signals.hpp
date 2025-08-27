/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signals.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:52 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/26 18:45:11 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_HPP
#define SIGNALS_HPP

#include "Webserv.hpp"

void	sigQuitHandler(int signal);
void	sigIntHandler(int signal);

#endif