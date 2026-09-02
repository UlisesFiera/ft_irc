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
	client.removeChannel(*_channels[channel_name]);
}