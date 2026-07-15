#include "message/Message.hpp"

Message::Message(std::string message)
{
	if (!message.empty() && message[message.size() - 1] == '\n') message.erase(message.size() - 1);
	if (!message.empty() && message[message.size() - 1] == '\r') message.erase(message.size() - 1);

	size_t trailing_pos = message.find(" :");
	if (trailing_pos != std::string::npos)
	{
		_trailing = message.substr(trailing_pos + 2);
		message = message.substr(0, trailing_pos); 
	}

	size_t start = 0;
	size_t end = message.find(' ');

	if (end != std::string::npos)
	{
		_command = message.substr(start, end - start);
		start = end + 1;
		
		while ((end = message.find(' ', start)) != std::string::npos)
		{
			if (end != start) {
				_params.push_back(message.substr(start, end - start));
			}
			start = end + 1;
		}
		
		if (start < message.size()) {
			_params.push_back(message.substr(start));
		}
	}
	else
	{
		_command = message;
	}
}

Message::~Message() {}



std::string	Message::getCommand() const
{
	return _command;
}

std::vector<std::string> Message::getParams() const
{
	return _params;
}

std::string	Message::getTrailing() const
{
	return _trailing;
}