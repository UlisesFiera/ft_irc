#include "server/Server.hpp"

void Server::channelTopic(Client &client, const Message &message)
{
    if (message.getParams().empty())
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

    Channel *channel = _channels[channel_name];

    if (message.getParams().size() == 1 && message.getTrailing() == "")
    {
        if (channel->getTopic() == "")
            client.setResponse(Response(client, message, RPL_NOTOPIC));
        else
            client.setResponse(Response(client, message, RPL_TOPIC));
        return;
    }

    if (channel->getTopicRestricted() == true && !channel->isOperator(client.getNick()))
    {
        client.setResponse(Response(client, message, ERR_CHANOPRIVSNEEDED));
        return;
    }

    std::string new_topic = message.getTrailing();
    
 
    if (new_topic == "" && message.getParams().size() > 1) {
        new_topic = message.getParams()[1];
    }

    channel->setTopic(new_topic);

    createStreamingResponse(client, message, channel->getNicks());
	createStreamingResponse(client, message, client.getFd());
}