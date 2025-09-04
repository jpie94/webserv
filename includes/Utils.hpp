/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:28:12 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/04 14:36:23 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>

std::string	i_to_string(int value);

size_t		findCRLFCRLF(std::string str);

size_t		findCRLF(std::string str);

int			hexStringToInt(std::string str);

#endif