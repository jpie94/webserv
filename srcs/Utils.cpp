/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:27:35 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/02 11:19:45 by qsomarri         ###   ########.fr       */
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

/*size_t find_mem(char* &str1, const std::string &str2, size_t len)
{
	if (str1 && (str2.empty() || len < str2.size()))
		return (std::string::npos);
	for (size_t i = 0; i <= len - str2.size(); ++i)
		if (std::memcmp(&str1[i], &str2[0], str2.size()) == 0)
			return (i);
	return (std::string::npos);
}*/

size_t	find_mem(const std::vector<char>& vect, const std::string& str)
{
	size_t vect_len = vect.size();
	size_t str_len = str.size();
	
	if (vect.empty() || str.empty() || vect_len < str_len)
		return (std::string::npos);
	for (size_t i = 0; i <= vect_len - str_len; ++i)
	{
		if (std::memcmp(&vect.data()[i], str.data(), str_len) == 0)
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

// /*char*	memjoin(char* str1, char* str2, size_t str1_len, size_t sep_len)
// {
// 	char *tmp = NULL;
// 	if (str1)
// 	{
// 		tmp = new char[str1_len];
// 		std::memcpy(tmp, str1, str1_len);
// 		delete [] str1;
// 	}
// 	str1 = new char[str1_len + sep_len];
// 	if (tmp)
// 	{
// 		std::memcpy(str1, tmp, str1_len);
// 		delete [] tmp;
// 	}
// 	std::memcpy(&str1[str1_len], str2, sep_len);
// 	return (str1);
// }*/

// char*	memjoin(char* str1, const char* str2, size_t str1_len, size_t str2_len)
// {
// 	if (!str2 || str2_len == 0)
// 		return (str1);
// 	char*	res = new char[str1_len + str2_len];
// 	if (str1 && str1_len > 0)
// 		std::memcpy(res, str1, str1_len);
// 	std::memcpy(res + str1_len, str2, str2_len);
// 	if (str1)
// 		delete[] str1;
// 	return (res);
// }

// /*char*	submem(char* &str1, const std::string &sep, size_t len)
// {
// 	char*	dest = NULL;
// 	size_t	pos = find_mem(str1, sep, len);
// 	size_t	sep_len = sep.size();

// 	if (pos == std::string::npos)
// 		return (str1);
// 	if (pos + sep_len < len)
// 	{
// 		dest = new char[len - pos - sep_len];
// 		std::memcpy(dest, &str1[pos + sep_len], len - pos - sep_len);
// 	}
// 	return (dest);
// }*/

// char*	submem(const char* str1, const std::string& sep, size_t len, size_t& dest_len)
// {
// 	size_t pos = find_mem(str1, sep, len);
// 	if (pos == std::string::npos)
// 	{
// 		dest_len = 0;
// 		return (NULL);
// 	}
// 	size_t sep_len = sep.size();
// 	if (pos + sep_len >= len)
// 	{
// 		dest_len = 0;
// 		return (NULL);
// 	}
// 	dest_len = len - pos - sep_len;
// 	char* dest = new char[dest_len];
// 	std::memcpy(dest, &str1[pos + sep_len], dest_len);
// 	return (dest);
// }

void	printVect(std::vector<char>& v)
{
	for(std::vector<char>::iterator it = v.begin(); it != v.end(); ++it)
		std::cout << *it;
	std::cout << std::endl;
}