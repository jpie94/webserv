/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:16 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/23 17:51:19 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Request.hpp"
#include "Signals.hpp"
#include "Response.hpp"

// int	main()
// {
// 	try
// 	{
// 		Webserv ws;
// 		std::signal(SIGINT, sigIntHandler);
// 		std::signal(SIGQUIT, sigQuitHandler);
// 		ws.runWebserv();
// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 	}
// 	return (0);
// }

/* Request Test main */

int main(int argc, char** argv)
{
	if (argc != 2)
		return (1);
	try
	{
		Request r(argv[1]);
		r.parsRequest();
		r.makeResponse();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}