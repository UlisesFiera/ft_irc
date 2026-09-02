#include "server/Server.hpp"

void Server::channelMode(Client &client, const Message &message)
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

    Channel *channel = _channels[channel_name];

    if (message.getParams().size() == 1)
    {
        std::string active_modes = "";

        if (_channels[channel_name]->getInviteOnly() || _channels[channel_name]->getTopicRestricted() || _channels[channel_name]->getPassword() != "" || _channels[channel_name]->getUserLimit() > 0)
            active_modes += "+";

        if (_channels[channel_name]->getInviteOnly()) active_modes += "i";
        if (_channels[channel_name]->getTopicRestricted()) active_modes += "t";
        if (_channels[channel_name]->getPassword() != "") active_modes += "k";
        if (_channels[channel_name]->getUserLimit() > 0) active_modes += "l";

        _channels[channel_name]->setChannelModes(active_modes);

		client.setResponse(Response(client, message, RPL_CHANNELMODEIS));
        return;
    }

    if (!channel->isOperator(client.getNick()))
    {
        client.setResponse(Response(client, message, ERR_CHANOPRIVSNEEDED));
        return;
    }

    std::string mode_str = message.getParams()[1];

    bool adding = true;
    size_t arg_index = 2;

    std::string applied_modes = "";
    std::string applied_args = "";

    for (size_t i = 0; i < mode_str.length(); i++)
    {
        char c = mode_str[i];

        if (c == '+')
        {
            adding = true;
            if (applied_modes.empty() || applied_modes[applied_modes.length() - 1] != '+')
                applied_modes += "+";
            continue;
        }
        else if (c == '-')
        {
            adding = false;
            if (applied_modes.empty() || applied_modes[applied_modes.length() - 1] != '-')
                applied_modes += "-";
            continue;
        }

        else if (c != 'i' && c != 't' && c != 'k' && c != 'l' && c != 'o')
        {
            client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
            return;
        }

        bool mode_changed = false;

        switch (c)
        {
            case 'i':
                channel->setInviteOnly(adding);
                mode_changed = true;
                break;

            case 't':
                channel->setTopicRestricted(adding);
                mode_changed = true;
                break;

            case 'k':
                if (adding) {
                    if (arg_index < message.getParams().size()) {
                        channel->setPassword(message.getParams()[arg_index]);
                        applied_args += " " + message.getParams()[arg_index];
                        arg_index++;
                        mode_changed = true;
                    }
                } else {
                    channel->setPassword("");
                    mode_changed = true;
                }
                break;

            case 'l':
                if (adding) {
                    if (arg_index < message.getParams().size()) {
                        int limit = std::atoi(message.getParams()[arg_index].c_str());
                        channel->setUserLimit(limit);
                        applied_args += " " + message.getParams()[arg_index];
                        arg_index++;
                        mode_changed = true;
                    }
                } else {
                    channel->setUserLimit(0);
                    mode_changed = true;
                }
                break;

            case 'o':
                if (arg_index < message.getParams().size()) {
                    std::string target_nick = message.getParams()[arg_index];
                    if (adding && !channel->isOperator(target_nick)) {
                        channel->setOperator(target_nick);
                        mode_changed = true;
                    } else if (!adding && channel->isOperator(target_nick)) {
                        channel->removeOperator(target_nick);
                        mode_changed = true;
                    }
                    applied_args += " " + target_nick;
                    arg_index++;
                }
                break;

            default:
                break;
        }

        if (mode_changed) {
            applied_modes += c;
        }
    }

    if (applied_modes != "" && applied_modes != "+" && applied_modes != "-")
    {
        createStreamingResponse(client, message, channel->getNicks());
	    createStreamingResponse(client, message, client.getFd());
    }
}