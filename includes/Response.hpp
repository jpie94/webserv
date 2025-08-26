/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:55 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/26 16:11:31 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <ctime>
#include <fcntl.h>
#include "Request.hpp"

#define SERVER_ROOT "root/"

class Response : public Request
{
	private:
		std::string									_response;
		std::string									_fileName;
		std::string									_responseBody;
		static std::map<std::string, std::string>	_types;
		bool										_autoIndex;//Inherited from Server Class
	public:
		/* Canonical Form + Paramtric constructor */
													Response();
													Response(Request&);
													Response(const Response&);
		Response&									operator=(const Response&);
		virtual										~Response();
		/* Member Functions */
		void										getMethode();
		void										postMethode();
		void										deleteMethode();
		void										autoIndex();
		void										readFile();
		std::string									getContent_type();
		std::string									getFileType();
		void										HandlePath();
		void										callMethode();
		std::string									getTime() const;
		void										printResponse() const;//del
		std::string									getFileExt(std::string value) const;
};
#endif