#include "server/Server.hpp"

bool Server::checkChannel(const std::string &name)
{
    if (_channels.find(name) != _channels.end())
        return true;
    return false;
}

void Server::joinChannel(Client &client, const Message &message)
{
	if (message.getParams().size() != 1 && message.getParams().size() != 2)
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		return;
	}

	std::string channel_name = message.getParams()[0];

	if (!(checkChannel(channel_name)))
	{
		try
		{
			Channel *new_channel = new Channel(channel_name);

			_channels[channel_name] = new_channel;
			std::cout << "[JOIN] Channel " << channel_name << " created by " << client.getNick() << std::endl;
			_channels[channel_name]->setOperator(client.getNick());
		}
        
        catch(const std::exception& e)
        {
            client.setResponse(Response(client, message, ERR_NOSUCHCHANNEL));
            return;
        }
    }
	else
	{
		if (_channels[channel_name]->getInviteOnly() && !_channels[channel_name]->isInvited(client.getNick()))
		{
			client.setResponse(Response(client, message, ERR_INVITEONLYCHAN));
			return;
		}

		if (_channels[channel_name]->getPassword() != "" && _channels[channel_name]->getInviteOnly())
		{
			if (message.getParams()[1] == "" || !_channels[channel_name]->checkPassword(message.getParams()[1]))
			{
				client.setResponse(Response(client, message, ERR_BADCHANNELKEY));
				return;
			}
		}

		if (_channels[channel_name]->getUserLimit() != 0 && _channels[channel_name]->getUserLimit() >= _channels[channel_name]->getMembersVec().size())
		{
			client.setResponse(Response(client, message, ERR_CHANNELISFULL));
			return;
		}

		if (_channels[channel_name]->getInviteOnly() == true)
			_channels[channel_name]->removeInvitation(client.getNick());
	}

	if (client.isInChannel(channel_name))
		return;

	std::cout << "[JOIN] Client " << client.getNick() << " joined channel " << channel_name << std::endl;

	_channels[channel_name]->setMembers(&client);
	client.setChannel(_channels[channel_name]);
	createStreamingResponse(client, message, _channels[channel_name]->getNicks());
	createStreamingResponse(client, message, client.getFd());
}