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
	std::string reason = message.getTrailing();

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

	if (!_channels[channel_name]->isInChannel(target))
	{
		client.setResponse(Response(client, message, ERR_USERNOTINCHANNEL));
		return;
	}

	if (reason.empty())
	{
		_channels[channel_name]->setKickReason("");
	}

	else
		_channels[channel_name]->setKickReason(reason);

	createStreamingResponse(client, message, _channels[channel_name]->getNicks());
	createStreamingResponse(client, message, client.getFd());
	_channels[channel_name]->getClientFromTarget(target)->removeChannel(*_channels[channel_name]);
	_channels[channel_name]->removeMember(*_channels[channel_name]->getClientFromTarget(target));
}
