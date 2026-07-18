#include "server/Server.hpp"

void	Server::sendPrivmsg(Client &client, const Message &message)
{
	std::map<int, Client>::iterator	it;
	std::string						target;

	if (message.getParams().size() != 2)
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		break ;
	}
	target = message.getParams()[0];
	if (target == client.getNick())

	if (target[0] == '#')
	{
		/*for (it = _clients.begin(); it != _clients.end(); it++)
		{
			if (it->second.getNick() == client.getNick()) // what if tries to send to himself
				continue ;
			if (it->second.getNick() == target)
				break ;
			client.setResponse(Response(client, message, ERR_NOSUCHCHANNEL));
			return ;
		}*/
	}
	else
	{
		for (it = _clients.begin(); it != _clients.end(); it++)
		{
			if (it->second.getNick() == client.getNick())
				continue ;
			if (it->second.getNick() == target)
				break ;
			client.setResponse(Response(client, message, ERR_NOSUCHNICK));
			return ;
		}
	}
	client.setResponse(Response(client, message));
}
