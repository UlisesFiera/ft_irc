#include "server/Server.hpp"

int	Server::nick2fd(const std::string &nick)
{
	std::map<int, Client*>::iterator	it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second->getNick() == nick)
			return (it->first);
	}
	return (-1);
}

size_t	Server::findcrfl(const std::string &stream)
{
	return (stream.find("\r\n"));
}

void	Server::createStreamingResponse(Client &client, const Message &message, const std::vector<std::string> &channel)
{
	int	target;

	for (size_t i = 0; i < channel.size(); i++)
	{	
		if (nick2fd(channel[i]) == client.getFd())
			continue ;
		target = nick2fd(channel[i]);
		_event_manager.update(target, POLLOUT);
		_clients[target]->setResponse(Response(client, message, target));
	}
}

void	Server::createStreamingResponse(Client &client, const Message &message, const int &target)
{
	if (target != client.getFd())
		_event_manager.update(target, POLLOUT);
	_clients[target]->setResponse(Response(client, message, target));
}
