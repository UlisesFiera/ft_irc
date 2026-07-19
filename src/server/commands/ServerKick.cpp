#include "server/Server.hpp"

void	Server::kickChannel(Client &client, const Message &message)
{
	if (message.getParams().size() != 2)
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		return;
	}

	std::string channel_name = message.getParams()[0];
	std::string target = message.getParams()[1];

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
	
	if (client.getChannel(channel_name)->isOperator(client.getNick()))
	{
		client.setResponse(Response(client, message, ERR_CHANOPRIVSNEEDED));
		return;
	}

	if (!client.getChannel(channel_name)->isOperator(target))
	{
		client.setResponse(Response(client, message, ERR_USERNOTINCHANNEL));
		return;
	}

	createStreamingResponse(client, message, _channels[channel_name]->getNicks());
	_channels[channel_name]->removeMembers(client);
	//client.setChannel(_channels[channel_name]);
}
