/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:27:35 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/05 17:13:26 by qsomarri         ###   ########.fr       */
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

size_t	findCRLF(std::string str)
{
	size_t	i = -1;
	while(str[++i])
	{
		if (str[i] == '\r' && str[i + 1] && str[i + 1] == '\n')
			return (i);
	}
	return (std::string::npos);
}

int	hexStringToInt(std::string str)
{
	int	res;
	std::istringstream(str) >> std::hex >> res;
	return (res);
}

void strCapitalizer(std::string &str)
{
	size_t i = -1;
	while (str[++i])
		str[i] = static_cast<char>(std::toupper(str[i]));
}

std::string trim_white_spaces(std::string str)
{
	std::istringstream	ss(str);
	std::string	res, tmp;

	while (ss >> tmp)
		res += tmp + " ";
	return (res.substr(0, res.size() - 1));
}

void	trim_CRLF(std::string &str)
{
	size_t	end = str.size() - 1;

	while (end - 1 > 0 && str[end] == '\n' && str[end - 1] == '\r')
	{
		str = str.substr(0, str.size() - 2);
		end = str.size() - 1;
	}
}