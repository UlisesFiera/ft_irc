#include "client/Client.hpp"

Client::Client()
{
	_lastActivity = time(NULL);
}

Client::Client(const Client &copyClient)
{
	_port = copyClient._port;
	_lastActivity = copyClient._lastActivity;
}

Client	&Client::operator=(const Client &copyClient)
{
	if (this != &copyClient)
	{
		_port = copyClient._port;
		_lastActivity = copyClient._lastActivity;
	}
	return (*this);
}

Client::~Client()
{

}

std::string const	&Client::getStream() const
{
	return (_stream);
}

time_t const	&Client::getLastActivity() const
{
	return (_lastActivity);
}

void Client::setLastActivity()
{
	_lastActivity = time(NULL);
}

void	Client::setPort(const int &port)
{
	_port = port;
}

void	Client::setStream(const std::string &stream)
{
	_stream = stream;
}
