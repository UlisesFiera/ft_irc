#include "server/Server.hpp"

void	Server::channelInvite(Client &client, const Message &message)
{
	if (message.getParams().size() != 2)
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		return;
	}

	std::string channel_name = message.getParams()[1];
	std::string target = message.getParams()[0];

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
	
	if (!_channels[channel_name]->isOperator(client.getNick()))
	{
		client.setResponse(Response(client, message, ERR_CHANOPRIVSNEEDED));
		return;
	}

	if (_channels[channel_name]->isInChannel(target))
	{
		client.setResponse(Response(client, message, ERR_USERONCHANNEL));
		return;
	}

	if (!_channels[channel_name]->isInChannel(target))
		_channels[channel_name]->setInvitedNicks(target);

	int target_fd = nick2fd(target);
    if (target_fd == -1)
    {
        client.setResponse(Response(client, Message("INVITE " + target), ERR_NOSUCHNICK));
        return;
    }

	createStreamingResponse(client, message, nick2fd(target));
	client.setResponse(Response(client, message, RPL_INVITING));
	return;
}