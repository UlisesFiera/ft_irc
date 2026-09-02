#include "server/Server.hpp"

void	Server::registerPass(Client &client, const Message &message)
{
	std::string	pass;

	if (message.getCommand() != PASS)
		return ;
	if (message.getParams().empty())
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		return ;
	}
	pass = message.getParams()[0];
	if (pass == _password)
		client.setPass(pass);
	else
	{
		client.setResponse(Response(client, message, ERR_PASSWDMISMATCH));
		return ;
	}
}

bool	Server::nickSyntax(const std::string &nick)
{
	if (nick.empty())
		return (false);
	
	char		c = nick[0];
	std::string	rem = nick.substr(1);

	if (nick.size() > NICK_SIZE)
		return (false);
	if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z') 
		&& std::string("[]\\_^{}|`").find(c) == std::string::npos)
		return (false);
	for (size_t i = 0; i < rem.size(); i++)
	{
		c = rem[i];
		if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z')
			&& !(c >= '0' && c <= '9') && c != '-'
			&& std::string("[]\\_^{}|`").find(c) == std::string::npos)
			return (false);
	}
	return (true);
}

bool	Server::registerNick(Client &client, const Message &message)
{
	std::map<int, Client*>::iterator	it;

	if (message.getCommand() != NICK)
		return (false);
	if (message.getParams().size() != 1)
	{
		client.setResponse(Response(client, message, ERR_NONICKNAMEGIVEN));
		return (false);
	}
	if (message.getParams().size() > 1 || !nickSyntax(message.getParams()[0]))
	{
		client.setResponse(Response(client, message, ERR_ERRONEUSNICKNAME));
		return (false);
	}
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getFd() == client.getFd())
			continue ;
		if (it->second->getNick() == message.getParams()[0])
		{
			client.setResponse(Response(client, message, ERR_NICKNAMEINUSE));
			return (false);
		}
	}
	client.setNick(message.getParams()[0]);
	return (true);
}

void	Server::registerUser(Client &client, const Message &message)
{
	std::string	user;
	std::string	real_name;

	if (message.getCommand() != USER)
		return ;
	if (message.getParams().size() != 3)
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		return ;
	}
	if (client.getUser() != "")
	{
		client.setResponse(Response(client, message, ERR_ALREADYREGISTERED));
		return ;
	}
	user = message.getParams()[0];
	real_name = message.getTrailing();
	client.setUser(user);
	client.setName(real_name);
}
