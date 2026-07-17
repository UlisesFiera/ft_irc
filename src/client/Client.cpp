#include "client/Client.hpp"

Client::Client()
{
	_lastActivity = time(NULL);
	_registered = false;
	_pass = "";
	_nick = "";
	_user = "";
	_host = "";
}

Client::Client(const Client &copyClient)
{
	_port = copyClient._port;
	_lastActivity = copyClient._lastActivity;
	_messages = copyClient._messages;
	_registered = copyClient._registered;
	_pass = copyClient._pass;
	_nick = copyClient._nick;
	_user = copyClient._user;
	_responses = copyClient._responses;
	_host = copyClient._host;
}

Client	&Client::operator=(const Client &copyClient)
{
	if (this != &copyClient)
	{
		_port = copyClient._port;
		_lastActivity = copyClient._lastActivity;
		_messages = copyClient._messages;
		_registered = copyClient._registered;
		_pass = copyClient._pass;
		_nick = copyClient._nick;
		_user = copyClient._user;
		_responses = copyClient._responses;
		_host = copyClient._host;
	}
	return (*this);
}

Client::~Client()
{

}
const std::string	&Client::getHost() const
{
	return (_host);
}

std::vector<Response>	&Client::getResponses()
{
	return (_responses);
}

const std::string	&Client::getPass() const
{
	return (_pass);
}

const std::string	&Client::getNick() const
{
	return (_nick);
}

const std::string	&Client::getUser() const
{
	return (_user);
}

bool const	&Client::isRegistered() const
{
	return (_registered);
}

std::vector<Message>	&Client::getMessages()
{
	return (_messages);
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

void	Client::setMessage(const std::string &stream)
{
	Message	message = Message(stream);

	_messages.push_back(message);
}

void	Client::setRegistered(const bool &status)
{
	_registered = status;
}

void	Client::setPass(const std::string &pass)
{
	_pass = pass;
}

void	Client::setNick(const std::string &nick)
{
	_nick = nick;
}

void	Client::setUser(const std::string &user)
{
	_user = user;
}

void	Client::setResponse(const Response &response)
{
	_responses.push_back(response);
}

void	Client::setHost(const std::string &host)
{
	_host = host;
}
