#include "server/Server.hpp"

void	Server::sendPrivmsg(Client &client, const Message &message)
{
	std::map<int, Client>
		::iterator					it;
	std::map<std::string, Channel>
		::iterator					itc;
	std::string						target;

	if (message.getParams().size() != 2)
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		break ;
	}
	target = message.getParams()[0];
	if (target[0] == '#')
	{
		target.substr(1);
		for (size_t i = 0; client.getChannels().size(); i++)
		{
			if (target == client.getChannels()[i].getName())
			{
				createStreamingResponse(client, message, client.getChannels()[i].getMembers());
				return ;
			}
				
		}
	}
	else
	{
		for (it = _clients.begin(); it != _clients.end(); it++)
		{
			if (it->second.getNick() == target)
			{
				createStreamingResponse(client, message, nick2fd(target));
				return ;
			}
		}
		client.setResponse(Response(client, message, ERR_NOSUCHNICK));
			return ;
	}
}
