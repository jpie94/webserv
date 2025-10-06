/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 14:17:00 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/06 17:55:30 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookies.hpp"

std::string setCookies()
{
	std::string res("Set-Cookie: ");

	res += "\"" + getTime() + "_" + generateRandomName(4) + "\"=";
	res += "\"" + generateRandomName(10) + "\";";
	res += " Max-Ages=\"34560000\";";
	//res += "; Expires=\"" + getTimeStr() + 400 + "\";";
	res += "\r\n";
	return (res);
}