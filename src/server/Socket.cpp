#include "server/Socket.hpp"

Socket::Socket()
{

}

Socket::Socket(const int &port)
{
	_opt = 1;
	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(port);
	_socketFd = socket(_address.sin_family, SOCK_STREAM, 0);
	if (_socketFd == -1)
		throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &_opt, sizeof(_opt)) < 0)
		throw std::runtime_error(std::string("setsockopt: ") + std::strerror(errno));
	if (fcntl(_socketFd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error(std::string("fcntl: ") + std::strerror(errno));
	if (bind(_socketFd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
		throw std::runtime_error(std::string("bind: ") + std::strerror(errno));
	if (listen(_socketFd, SOMAXCONN) < 0)
		throw std::runtime_error(std::string("listen: ") + std::strerror(errno));
	std::cout << "Listening in " << inet_ntoa(_address.sin_addr)
             					 << ":" << ntohs(_address.sin_port)
             					 << std::endl;
}

Socket::Socket(const Socket &other)
{
	_socketFd = other._socketFd;
	_port = other._port;
	_address = other._address;
	_isListening = other._isListening;
	_opt = other._opt;
}

Socket& Socket::operator=(const Socket &other)
{
	if (this != &other)
	{
		_socketFd = other._socketFd;
		_port = other._port;
		_address = other._address;
		_isListening = other._isListening;
		_opt = other._opt;
	}
	return (*this);
}

Socket::~Socket() {}

// setters and getters

const int &Socket::getSocketFd() const
{
	return (_socketFd);
}

const int &Socket::getPort() const
{
	return (_port);
}

const sockaddr_in &Socket::getAddress() const
{
	return (_address);
}

const int &Socket::getIsListening() const
{
	return (_isListening);
}
