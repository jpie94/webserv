/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:28:12 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/02 11:19:22 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <cstring>

std::string	i_to_string(int value);

size_t		findCRLFCRLF(std::string str);

size_t		findCRLF(std::string str);

size_t		find_mem(const std::vector<char>& vect, const std::string &substr);

int			hexStringToInt(std::string str);

void		 strCapitalizer(std::string &str);

std::string trim_white_spaces(std::string str);

void		trim_CRLF(std::string &str);

void		removeQuotes(std::string& str);

std::string	getName(const std::string& str, const std::string& key);

std::string	generateRandomName();

// char*	memjoin(char* str1, char* str2, size_t str1_len, size_t str2_len);

// char*	submem(char* &str1, const std::string &str2, size_t len);
void	printVect(std::vector<char>&);

#endif