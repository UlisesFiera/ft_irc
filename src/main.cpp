#include <iostream>
#include <string>
#include <sstream>

#include "server/Server.hpp"

#define DEFAULT_PORT 6667
#define PASSWORD "1234abcd_"

int	main(int argc, char **argv)
{
	int					port;
	std::string			password;
	Server				server = Server();

	if (argc == 1)
	{
		port = DEFAULT_PORT;
		password = PASSWORD;
		server.run(port, password);
	}
	else if (argc == 2)
	{
		std::cerr << "Couldn't start ircserv. Expected arguments: ./ircserv <port> <password>\n";
		return (1);
	}
	else if (argc == 3)
	{
		port = std::atoi(argv[1]);
		password = argv[2];
		server.run(port, password);
	}
	else
	{
		std::cerr << "Couldn't start ircserv. Expected arguments: ./ircserv <port> <password>\n";
		return (1);
	}
	return (0);
}