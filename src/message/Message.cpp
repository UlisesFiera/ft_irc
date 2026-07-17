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
	else
		return INVALID;
}

void	Message::findTrailing()
{
	size_t	trailing_pos = _stream.find(" :");

	if (trailing_pos != std::string::npos)
	{
		_trailing = _stream.substr(trailing_pos + 2);
		_stream = _stream.substr(0, trailing_pos); 
	}
}

void	Message::parseMessage()
{
	size_t	end = _stream.find(' ');
	size_t	start = 0;

	if (end != std::string::npos)
	{
		_command = _stream.substr(start, end - start);
		start = end + 1;
		while ((end = _stream.find(' ', start)) != std::string::npos)
		{
			if (end != start)
				_params.push_back(_stream.substr(start, end - start));
			start = end + 1;
		}
		if (start < _stream.size())
			_params.push_back(_stream.substr(start));
	}
	else
		_command = _stream;
}

void	Message::parse()
{
	parseMessage();
	findTrailing();
	_cmd = resolveCommand();
}
