#include <iostream>
#include <string>
#include <sstream>

#include "server/Server.hpp"

#define DEF_PORT 6667
#define DEF_PASSWORD "1234abcd_"

bool	validPass(const char *pass)
{
	std::string	spass(pass);

	if (spass.size() > 10)
	{
		std::cerr << "\033[31m[●]\033[0m Couldn't start server: invalid pass (too long)\n";
		return (false);
	}
	for (size_t i = 0; i < spass.size(); i++)
	{
		if (std::isprint(spass[i]) == 0)
		{
			std::cerr << "\033[31m[●]\033[0m Couldn't start server: invalid pass\n";
			return (false);
		}
	}
	return (true);
}

int	str2port(const char *port)
{
	std::string	sport(port);
	int			nport = -1;

	if (sport.size() > 5)
	{
		std::cerr << "\033[31m[●]\033[0m Couldn't start server: invalid port\n";
		return (nport);
	}
	for (size_t i = 0; i < sport.size(); i++)
	{
		if (sport[i] < '0' || sport[i] > '9')
		{
			std::cerr << "\033[31m[●]\033[0m Couldn't start server: invalid port\n";
			return (nport);
		}
	}
	nport = std::atoi(sport.c_str());
	return (nport);
}

int	main(int argc, char **argv)
{
	int					port;
	std::string			password;
	Server				server = Server();

	if (argc == 1)
	{
		port = DEF_PORT;
		password = DEF_PASSWORD;
		server.run(port, password);
	}
	else if (argc == 2)
	{
		std::cerr << "\033[31m[●]\033[0m Couldn't start ircserv. Expected arguments: ./ircserv <port> <password>\n";
		return (1);
	}
	else if (argc == 3)
	{
		port = str2port(argv[1]);
		if (port == -1)
			return (1);
		if (!validPass(argv[2]))
			return (1);
		password = argv[2];
		server.run(port, password);
	}
	else
	{
		std::cerr << "\033[31m[●]\033[0m Couldn't start ircserv. Expected arguments: ./ircserv <port> <password>\n";
		return (1);
	}
	return (0);
}
