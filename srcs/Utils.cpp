/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:27:35 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/30 15:31:23 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

std::string	i_to_string(int value)
{
	std::ostringstream	oss;
	oss << value;
	return (oss.str());
}

size_t	findCRLFCRLF(std::string str)
{
	size_t	i = -1;
	while(str[++i])
	{
		if (str[i] == '\r' && str[i + 1] && str[i + 1] == '\n'
				&& str[i + 2] && str[i + 2] == '\r' && str[i + 3] && str[i + 3] == '\n')
			return (i);
	}
	return (std::string::npos);
}