#include "utils/utils.hpp"

void	printcrlf(const std::string &str, size_t bytes)
{
	for (size_t i = 0; i < bytes; i++)
	{
		if (str[i] == '\r')
			std::cout << "\\r";
		else if (str[i] == '\n')
			std::cout << "\\n";
		else if (str[i] == '\t')
			std::cout << "\\t";
		else
			std::cout << str[i];
	}
	std::cout << std::endl;
}

std::string	getCommandString(const commands &command)
{
	std::string	cmd;

	switch (command)
	{
		case CAP:
			return (cmd = "CAP");
		case PRIVMSG:
			return (cmd = "PRIVMSG");
		case NICK:
			return (cmd = "NICK");
		case PASS:
			return (cmd = "PASS");
		case USER:
			return (cmd = "USER");
		case JOIN:
			return (cmd = "JOIN");
		case INVITE:
			return (cmd = "INVITE");
		case TOPIC:
			return (cmd = "TOPIC");
		case MODE:
			return (cmd = "MODE");
		case KICK:
			return (cmd = "KICK");
		case PING:
			return (cmd = "PING");
		case PONG:
			return (cmd = "PONG");
		default:
			return (cmd = "UNKNOWN");
	}
}
