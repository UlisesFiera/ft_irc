#include "response/Response.hpp"

//// cannon methods
// constructors

Response::Response()
{
	_addr = "hostname";
	_reply_code = ERR_UNKNOWNCOMMAND;
	_command = NONE;
	_nick = "*";
	_user = "*";
	_message = "";
	_destination = "";
	_response = "";
	_bytes_sent = 0;
}

Response::Response(const Client &client, const Message &message, const ReplyCode &code)
{
	_targets.push_back(client.getFd());
	_bytes_sent = 0;
	_reply_code = code;
	_addr = client.getAddr();
	_nick = client.getNick();
	_user = client.getUser();
	for (size_t i = 0; i < message.getParams().size(); i++)
		_params += message.getParams()[i] + " ";
	_command = message.getCommand();
	_trailing = message.getTrailing();
	if (_nick == "")
		_nick = "*";
	if (_user == "")
		_user = "*";
	buildNumericResponse();
}

Response::Response(const Client &client, const Message &message, const int &target)
{
	_targets.push_back(target);
	_bytes_sent = 0;
	_addr = client.getAddr();
	_nick = client.getNick();
	_user = client.getUser();
	for (size_t i = 0; i < message.getParams().size(); i++)
		_params += message.getParams()[i] + " ";
	_command = message.getCommand();
	_trailing = message.getTrailing();
	if (_nick == "")
		_nick = "*";
	if (_user == "")
		_user = "*";
	buildStreamingResponse(message);
}

Response::Response(const Client &client, const Message &message, const std::vector<int> &targets)
{
	_targets = targets;
	_bytes_sent = 0;
	_addr = client.getAddr();
	_nick = client.getNick();
	_user = client.getUser();
	for (size_t i = 0; i < message.getParams().size(); i++)
		_params += message.getParams()[i];
	_command = message.getCommand();
	_trailing = message.getTrailing();
	if (_nick == "")
		_nick = "*";
	if (_user == "")
		_user = "*";
	buildStreamingResponse(message);
}

// copy constructors

Response::Response(const Response& copyResponse)
{
	_addr = copyResponse._addr;
	_reply_code = copyResponse._reply_code;
	_command = copyResponse._command;
	_nick = copyResponse._nick;
	_user = copyResponse._user;
	_message = copyResponse._message;
	_destination = copyResponse._destination;
	_response = copyResponse._response;
	_bytes_sent = copyResponse._bytes_sent;
	_params = copyResponse._params;
	_trailing = copyResponse._trailing;
	_targets = copyResponse._targets;
}

// operator overrides

Response& Response::operator=(const Response& copyResponse)
{
	if (this != &copyResponse)
	{
		_addr = copyResponse._addr;
		_reply_code = copyResponse._reply_code;
		_command = copyResponse._command;
		_nick = copyResponse._nick;
		_user = copyResponse._user;
		_message = copyResponse._message;
		_destination = copyResponse._destination;
		_response = copyResponse._response;
		_bytes_sent = copyResponse._bytes_sent;
		_params = copyResponse._params;
		_trailing = copyResponse._trailing;
		_targets = copyResponse._targets;
	}
	return (*this);
}

// destructors

Response::~Response()
{

}

//// non-canon methods

std::vector<int>	&Response::getTargets()
{
	return (_targets);
}

const size_t	&Response::getBytesSent() const
{
	return (_bytes_sent);
}

const std::string	&Response::getResponse() const
{
	return (_response);
}

void	Response::setBytesSent(const int &bytes)
{
	_bytes_sent = bytes;
}

std::string	Response::getCommandString(const commands &command)
{
	std::string	cmd;

	switch (command)
	{
		case CAP:
			return (cmd = "CAP");
		case PRIVMSG:
			return (cmd = "PRIVMSG");
		case NICK:
			return (cmd = "NICK");
		case PASS:
			return (cmd = "PASS");
		case USER:
			return (cmd = "USER");
		case JOIN:
			return (cmd = "JOIN");
		case INVITE:
			return (cmd = "INVITE");
		case TOPIC:
			return (cmd = "TOPIC");
		case MODE:
			return (cmd = "MODE");
		case KICK:
			return (cmd = "KICK");
		case PING:
			return (cmd = "PING");
		case PONG:
			return (cmd = "PONG");
		case PART:
			return (cmd = "PART");
		default:
			return (cmd = "UNKNOWN");
	}
}

void	Response::buildStreamingResponse(const Message &message)
{
	std::string	response;
	std::string prefix = ":" + _nick + "!" + _user + "@" + _addr;

	response = prefix + " " + getCommandString(_command);
	if (_command == PONG)
	{
		response = "PONG :irc.asulgernan.lol\r\n";
		_response = response;
		return ;
	}
	if (_command == CAP)
	{
		if (message.getParams()[0] == "END")
		{
			std::cout << "CAP negotiation ended" << std::endl;
			return ;
		}
		response = ":irc.asulgernan.lol CAP * LS :\r\n";
		_response = response;
		return ;
	}
	if (_command != NICK && _command != INVITE)
	{	
		if (!_params.empty())
			response += " " + _params;
		if (!_trailing.empty())
			response += " :" + _trailing;
	}
	else if (_command != NICK)
	{
		_trailing = message.getParams()[0];
		response += " :" + _trailing;
	}
	else if (_command != INVITE)
	{
		_trailing = message.getParams()[0];
		response += " :" + _trailing;
	}
	response += "\r\n";
	_response = response;
}

std::string	Response::header()
{
	std::string	header;
	std::string	code = std::to_string(_reply_code);

	if (code.size() == 1)
		code.insert(0, "00");
	header = ":irc.asulgernan.lol " + code + " " + _nick + " ";
	return (header);
}

void	Response::buildNumericResponse()
{
	std::string	response = header();

	switch (_reply_code)
	{
		case RPL_WELCOME:
			response += ":Welcome to the Internet Relay Network " + _nick + "!" + _user + "@" + _addr;
			break ;
		case RPL_YOURHOST:
			response += ":Your host is irc.asulgernan.lol, running version ft_irc-1.0";
			break ;
		case RPL_CREATED:
			response += ":This server was created Jul 23 2026";
			break ;
		case RPL_MYINFO:
			response += "irc.asulgernan.lol ft_irc-1.0 oi o";
			break ;
		case RPL_ISUPPORT:
			response += "CHANTYPES=# "
				"PREFIX=(o)@ "
				"CHANMODES,,, "
				"NICKLEN=30 "
				"NETWORK=ft_irc "
				":are supported by this server";
			break ;
		case ERR_NONICKNAMEGIVEN:
			response += ":No nickname given";
			break ;
		case ERR_ERRONEUSNICKNAME:
			response += _params + " :Erroneous nickname";
			break ;
		case ERR_NICKNAMEINUSE:
			response += _params + " :Nickname is already in use";
			break ;
		case ERR_NEEDMOREPARAMS:
			response += _params + " :Not enough parameters";
			break ;
		case ERR_ALREADYREGISTERED:
			response += ":You may not reregister";
			break ;
		case ERR_PASSWDMISMATCH:
			response += ":Password incorrect";
			break ;
		case ERR_UNKNOWNCOMMAND:
			response += _params + " :Unknown command";
			break ;
		case ERR_NOTREGISTERED:
			response += ":You have not registered";
			break ;
		case ERR_NOSUCHCHANNEL:
			response += _params + " :No such channel";
			break ;
		case ERR_NOSUCHNICK:
			response += _params + " :No such nick/channel";
			break ;
		case ERR_NOTONCHANNEL:
			response += _params + " :You're not on that channel";
			break ;
		case ERR_USERONCHANNEL:
			response += _params + " :is already on channel";
			break ;
		case ERR_CHANOPRIVSNEEDED:
			response += _params + " :You're not channel operator";
			break ;
		case ERR_CANNOTSENDTOCHAN:
			response += _params + " :Cannot send to channel";
			break ;
		case ERR_INVITEONLYCHAN:
			response += _params + " :Cannot join channel (+i)";
			break ;
		case ERR_CHANNELISFULL:
			response += _params + " :Cannot join channel (+l)";
			break ;
		case ERR_BADCHANNELKEY:
			response += _params + " :Cannot join channel (+k)";
			break ;
		case ERR_UNKNOWNMODE:
			response += _params + " :is unknown mode char to me";
			break ;
		case RPL_CHANNELMODEIS:
			response += _params;
			break ;
		default:
			response += ":Unknown reply";
			break ;
	}

	response += "\r\n";
	_response = response;
}
