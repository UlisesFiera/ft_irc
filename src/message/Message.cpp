#include "message/Message.hpp"

Message::Message()
{
	_trailing = "";
	_cmd = NONE;
}

Message::Message(std::string stream)
{
	_trailing = "";
	_stream = stream;
	_cmd = NONE;
	parse();
}

Message::Message(const Message &copyMessage)
{
	_command = copyMessage._command;
	_params = copyMessage._params;
	_trailing = copyMessage._trailing;
	_cmd = copyMessage._cmd;
}

Message &Message::operator=(const Message &copyMessage)
{
	if (this != &copyMessage)
	{
		_command = copyMessage._command;
		_params = copyMessage._params;
		_trailing = copyMessage._trailing;
		_cmd = copyMessage._cmd;
	}
	return (*this);
}

Message::~Message() {}

commands	Message::getCommand() const
{
	return _cmd;
}

std::vector<std::string> Message::getParams() const
{
	return _params;
}

std::string	Message::getTrailing() const
{
	return _trailing;
}

commands Message::resolveCommand()
{
	if (_command == "CAP")
		return CAP;
	if (_command == "NICK")
		return NICK;
	if (_command == "PASS")
		return PASS;
	if (_command == "USER")
		return USER;
	if (_command == "JOIN")
		return JOIN;
	if (_command == "PRIVMSG")
		return PRIVMSG;
	if (_command == "KICK")
		return KICK;
	if (_command == "INVITE")
		return INVITE;
	if (_command == "TOPIC")
		return TOPIC;
	if (_command == "MODE")
		return MODE;
	if (_command == "PING")
		return PING;
	if (_command == "PONG")
		return PONG;
	else
		return INVALID;
}

void	Message::parseMessage()
{
	size_t	next = _stream.find(' ');
	size_t	start = 0;

	if (_stream.empty())
		return ;
	if (next != std::string::npos)
		_command = _stream.substr(start, next - start);
	else
	{
		_command = _stream;
		return ;
	}
	start = next + 1;
	while (start < _stream.size())
	{
		next = _stream.find(' ', start);
		if (next == std::string::npos)
		{
			if (_stream[start] == ':')
				_trailing = _stream.substr(start + 1);
			else
				_params.push_back(_stream.substr(start));
			return ;
		}
		if (_stream[start] == ':')
		{
			_trailing = _stream.substr(start+ 1);
			return ;
		}
		_params.push_back(_stream.substr(start, next - start));
		start = next + 1;
	}
}

void	Message::parse()
{
	parseMessage();
	_cmd = resolveCommand();
}
