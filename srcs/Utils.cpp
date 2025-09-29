/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:27:35 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/29 16:39:42 by qsomarri         ###   ########.fr       */
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

void removeQuotes(std::string& str)
{
	if (str.size() >= 2 && ((str[0] == '"' && str[str.size() - 1] == '"')
		|| (str[0] == '\'' && str[str.size() - 1] == '\'')))
		str = str.substr(1, str.size() - 2);
}

std::string getName(const std::string& str, const std::string& key)
{
	size_t	pos = str.find(key);

	if (pos == std::string::npos)
		return ("");
	std::string	name = str.substr(pos + key.size());
	size_t	sep = name.find(';');
	if (sep != std::string::npos)
		name = name.substr(0, sep);
	removeQuotes(name);
	return (name);
}

std::string generateRandomName()
{
	size_t	len = 10;
	std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string result;
	std::srand(time(0));
	for (size_t i = 0; i < len; ++i)
		result += chars[std::rand() % chars.size()];
	return (result);
}