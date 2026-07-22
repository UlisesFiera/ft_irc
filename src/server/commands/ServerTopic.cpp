#include "server/Server.hpp"

void Server::channelTopic(Client &client, const Message &message)
{
	if (message.getParams().size() != 1)
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		return;
	}

	std::string channel_name = message.getParams()[0];
	std::string new_topic = message.getTrailing();

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
	
	if (!client.getChannel(channel_name)->isOperator(client.getNick()))
	{
		client.setResponse(Response(client, message, ERR_CHANOPRIVSNEEDED));
		return;
	}

	if (message.getParams().size() == 1 && message.getTrailing() == "" && client.getChannel(channel_name)->getTopic() == "")
	{
		client.setResponse(Response(client, message, RPL_NOTOPIC));
		return;
	}

	if (message.getParams().size() == 1 && message.getTrailing() == "" && client.getChannel(channel_name)->getTopic() != "")
	{
		client.setResponse(Response(client, message, RPL_TOPIC));
		return;
	}

	if (message.getParams().size() == 1 && message.getTrailing() != "")
	{
		client.getChannel(channel_name)->setTopic(new_topic);
		createStreamingResponse(client, message, _channels[channel_name]->getNicks());
		createStreamingResponse(client, message, client.getFd());
		return;
	}

	else
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		return;
	}
}