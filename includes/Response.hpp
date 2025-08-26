/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:55 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/26 16:30:23 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include <map>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_ROOT "root/"

class Response : public Request
{
	private:
		std::string					_response;
		std::string					_fileName;
		std::string					_responseBody;
		static std::map<std::string, std::string>	_types;
		bool						_autoIndex;//Inherited from Server Class
	public:
		/* Canonical Form + Paramtric constructor */
								~Response();
								Response();
								Response(Request&);
								Response(const Response&);
		Response&					operator=(const Response&);
		/* Member Functions */
		void						getMethode();
		void						postMethode();
		void						deleteMethode();
		void						autoIndex();
		void						readFile();
		std::string					getContent_type();
		std::string					getFileType();
		void						HandlePath();
		void						callMethode();
		std::string					getTime() const;
		void						printResponse() const;//del
		std::string					getFileExt(std::string value) const;
		void						setResponse();
		void						setErrorPage();
};