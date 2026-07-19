#include "events/EventManager.hpp"

//// cannon methods
// constructors

EventManager::EventManager()
{

}

// copy constructors

EventManager::EventManager(const EventManager& copyEventManager)
{
	_pollfds = copyEventManager._pollfds;
	_socketfd2event = copyEventManager._socketfd2event;
}

// operator overrides

EventManager& EventManager::operator=(const EventManager& copyEventManager)
{
	if (this != &copyEventManager)
	{
		_pollfds = copyEventManager._pollfds;
		_socketfd2event = copyEventManager._socketfd2event;
	}
	return (*this);
}

// destructors

EventManager::~EventManager()
{

}

//// non-canon methods

pollfd*	EventManager::pollfds()
{
	return (_pollfds.empty() ? NULL : &_pollfds[0]);
}

size_t	EventManager::size() const
{
	return (_pollfds.size());
}

std::vector<pollfd> EventManager::vector()
{
	return (_pollfds);
}

void	EventManager::addEvent(const int fd, const short event)
{
	pollfd	pollfd;

	std::memset(&pollfd, 0, sizeof(pollfd));
	pollfd.fd = fd;
	pollfd.events = event;
	pollfd.revents = 0;
	_pollfds.push_back(pollfd);
	std::cout << "Socket " << fd << " added event " << event2str(event) << std::endl;
}

void	EventManager::addListen(int fd)
{
	addEvent(fd, POLLIN);
	_socketfd2event[fd] = LISTEN;
}

void	EventManager::addClient(int fd)
{
	addEvent(fd, POLLIN);
	_socketfd2event[fd] = CLIENT;
}

void	EventManager::addRFile(int fd)
{
	addEvent(fd, POLLIN);
	_socketfd2event[fd] = RFILE;
}

void	EventManager::addWFile(int fd)
{
	addEvent(fd, POLLOUT);
	_socketfd2event[fd] = WFILE;
}

void	EventManager::close(int fd)
{
	std::vector<pollfd>::iterator	it;

	for (it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (it->fd == fd)
		{
			::close(fd);
			_pollfds.erase(it);
			_socketfd2event.erase(fd);
			std::cout << "Socket " << fd << " closed" << std::endl;
			break ;
		}
	}
}

void	EventManager::update(int fd, short event)
{
	std::vector<pollfd>::iterator	it;

	for (it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (it->fd == fd)
		{
			std::cout << "Socket " << fd << " swapped to " << event2str(event) << std::endl;
			it->events |= event;
			break ;
		}
	}
}

void	EventManager::clear(int fd, short event)
{
	std::vector<pollfd>::iterator	it;

	for (it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (it->fd == fd)
		{
			std::cout << "Socket " << fd << " cleared " << event2str(event) << std::endl;
			it->events &= ~event;
			break ;
		}
	}
}

bool	EventManager::isListen(int fd) const
{
	std::map<int, event>::const_iterator	it = _socketfd2event.find(fd);

	return (it != _socketfd2event.end() && it->second == LISTEN);
}

bool	EventManager::isClient(int fd) const
{
	std::map<int, event>::const_iterator	it = _socketfd2event.find(fd);

	return (it != _socketfd2event.end() && it->second == CLIENT);
}

bool	EventManager::isOutput(int fd) const
{
	std::map<int, event>::const_iterator	it = _socketfd2event.find(fd);

	return (it != _socketfd2event.end() && it->second == RFILE);
}

bool	EventManager::isInput(int fd) const
{
	std::map<int, event>::const_iterator	it = _socketfd2event.find(fd);

	return (it != _socketfd2event.end() && it->second == WFILE);
}

// setters and getters

std::vector<int>	EventManager::readableListen() const
{
	std::vector<int> 		sockets;
	std::vector<pollfd>
		::const_iterator	it;

	for (it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if ((it->revents & POLLIN) && isListen(it->fd))
			sockets.push_back(it->fd);
	}
	return (sockets);
}

std::vector<int>	EventManager::readableClients() const
{
	std::vector<int> 		sockets;
	std::vector<pollfd>
		::const_iterator	it;

	for (it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if ((it->revents & POLLIN) && isClient(it->fd))
			sockets.push_back(it->fd);
	}
	return (sockets);
}

std::vector<int>	EventManager::errorClients() const
{
	std::vector<int> 		sockets;
	std::vector<pollfd>
		::const_iterator	it;

	for (it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (((it->revents & POLLHUP) || (it->revents & POLLERR)) && isClient(it->fd))
			sockets.push_back(it->fd);
	}
	return (sockets);
}

std::vector<int>	EventManager::readableFiles() const
{
	std::vector<int> 		sockets;
	std::vector<pollfd>
		::const_iterator	it;

	for (it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if ((it->revents & POLLIN) && isOutput(it->fd))
			sockets.push_back(it->fd);
	}
	return (sockets);
}

std::vector<int>	EventManager::writableFiles() const
{
	std::vector<int> 		sockets;
	std::vector<pollfd>
		::const_iterator	it;

	for (it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if ((it->revents & POLLOUT) && isInput(it->fd))
			sockets.push_back(it->fd);
	}
	return (sockets);
}

std::vector<int>	EventManager::writableClients() const
{
	std::vector<int> 		sockets;
	std::vector<pollfd>
		::const_iterator	it;

	for (it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if ((it->revents & POLLOUT) && isClient(it->fd))
			sockets.push_back(it->fd);
	}
	return (sockets);
}

const std::string	event2str(short event)
{
	if (event == POLLIN)
		return ("POLLIN");
	if (event == POLLOUT)
		return ("POLLOUT");
	return ("INVALID");
}