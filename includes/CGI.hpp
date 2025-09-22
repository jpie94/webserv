/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:55 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/18 15:40:34 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <fcntl.h>
#include <limits>
#include "Request.hpp"

#define SERVER_ROOT ""
#define CRLFCRLF "\r\n\r\n"

class CGI : public Request
{
	private:
		char*							_varEnv[18];
		int									_In;
		int									_Out;
		int									_PID;

	public:
		/* Canonical Form + Paramtric constructor */
					CGI();
					CGI(Request &);
					CGI(const CGI &);
		CGI	&operator=(const CGI &);
		virtual		~CGI();
		/* Member Functions */
		void	fillVarEnv();
		void	newProcess();
		void	executeCGI();
		/* Getters & setters*/
		int 	get_FD_In ();
		int 	get_FD_Out ();
		int 	get_PID ();
};

#endif