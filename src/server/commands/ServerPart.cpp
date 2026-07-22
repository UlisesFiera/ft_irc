#include "server/Server.hpp"

void Server::channelPart(Client &client, const Message &message)
{
	if (message.getParams().size() != 1)
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		return;
	}

	std::string channel_name = message.getParams()[0];

	if (!checkChannel(channel_name))
	{
		client.setResponse(Response(client, message, ERR_NOSUCHCHANNEL));
		return;
	}

	if (!client.isInChannel(channel_name))
	{
		client.setResponse(Response(client, message, ERR_NOTONCHANNEL));
		return;
	}

	_channels[channel_name]->removeMember(client);

	if (_channels[channel_name]->isEmpty())
	{
		std::map<std::string, Channel*>::iterator it;

		for (it = _channels.begin(); it != _channels.end(); ++it)
		{
			if ((it->second)->getName() == _channels[channel_name]->getName())
			{
				delete it->second;
				_channels.erase(it);
				break;
			}
		}
	}
}