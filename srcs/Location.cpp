/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 13:59:58 by jpiech            #+#    #+#             */
/*   Updated: 2025/09/09 12:46:00 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/

Location::Location() : Server(){}

Location::Location(const Location &srcs)
{
	*this = srcs;
}

Location &Location::operator=(Location const &rhs)
{
	if (this != &rhs)
	{
		this->_config = rhs._config;
		this->_error_pages = rhs._error_pages;
		this->_cgi = rhs._cgi;
	}	
	return (*this);
}
Location::Location(std::string name) : Server(), _location_name(name) {}

Location::~Location() {}

/*****************	MEMBER		*******************/
void	Location::ExtractLocBloc(std::string & Config, size_t & it)
{
	std::string key, value;
	size_t	i = it;
	CheckBeforeBracket(Config, i);	
	while (Config[i])
	{
		key = GetConfigKey(Config, i);
		if(key.empty())
			break;			
		CheckDirective(key, true);
		value = GetConfigValue(Config, i);
		if(value.empty())
			break;
		if(key == "error_page")
			GetErrorPageConfig(value);
		else if (key == "cgi")
			GetCGIConfig(value);
		else
			this->_config[key] = value;
	}
	if(Config[i] && Config[i] == '}')
		{
			Config.erase(it, (i - it + 1));
		}
	else
			throw_error("Error in configuration file : unexpected end of file, expecting '}'.");
}
