/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:16 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/26 12:29:03 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Request.hpp"
#include "Signals.hpp"
#include "Response.hpp"

// int	main(int argc, char *argv[])
// {
// 	try
// 	{
// 		if(argc > 2)
// 			throw std::invalid_argument("Wrong number of arguments, must be 1 or 0");
// 		std::signal(SIGINT, sigIntHandler);
// 		std::signal(SIGQUIT, sigQuitHandler);
// 		Webserv ws(argv[1]);
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
	struct stat	path_stat;
	if (argc != 2)
		return 1;
	if (stat(argv[1], &path_stat) != 0)
		std::cout << "error" << std::endl;
	else
		std::cout << "ok" << std::endl;
	return(0);
	// if (argc != 2)
	// 	return (1);
	// try
	// {
	// 	Request r(argv[1]);
	// 	r.parsRequest();
	// 	r.makeResponse();
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// }
	// return (0);
}