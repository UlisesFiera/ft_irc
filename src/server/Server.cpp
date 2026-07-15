#include "server/Server.hpp"

//// Cannonical Methods
// Constructors

Server::Server() 
{

}

// Copy Constructors

Server::Server(const Server &other)
{
	_port = other._port;
	_password = other._password;
	_listening_socket = other._listening_socket;
}

// Operator Overrides

Server& Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_port = other._port;
		_password = other._password;
		_listening_socket = other._listening_socket;
	}
	return (*this);
}

// Destructors

Server::~Server() {}

// Functions

sig_atomic_t	stop_server = 0;

void	Server::signal_handler(int signum)
{
	if (signum == SIGINT)
		stop_server = 1;
}

void	Server::removeClients()
{
	const std::vector<int>		&error_clients = _event_manager.errorClients();

	for (size_t i = 0; i < error_clients.size(); i++)
	{
		_clients.erase(error_clients[i]);
		_event_manager.close(error_clients[i]);
		std::cout << "\033[32m✔\033[0m Client " << error_clients[i] << " disconnected." << std::endl;
	}
	for (size_t i = 0; i < _clients2rm.size(); i++)
	{
		_clients.erase(_clients2rm[i]);
		_event_manager.close(_clients2rm[i]);
		std::cout << "\033[32m✔\033[0m Client " << _clients2rm[i] << " disconnected." << std::endl;
	}
	_clients2rm.clear();
}

std::string	Server::readStream(int client_fd)
{
	char		buffer[BUFFER_SIZE];
	size_t		bytes_read = read(client_fd, buffer, BUFFER_SIZE);
	std::string	stored_stream = _clients[client_fd].getStream();

	if (bytes_read > 0)
	{
		stored_stream.append(buffer, bytes_read);
		_clients[client_fd].setLastActivity(); 
		return (stored_stream);
	}
	else if (bytes_read == 0)
		return ("");
	else
		throw std::runtime_error("Error: read() failed on client");
}

size_t	Server::findcrfl(const std::string &stream)
{
	return (stream.find("\r\n"));
}

void	Server::readClients()
{
	const std::vector<int>	&read_clients = _event_manager.readableClients();
	std::string				stream;
	size_t					pos;

	for (size_t i = 0; i < read_clients.size(); i++)
	{
		stream = "";
		try
		{
			stream = readStream(read_clients[i]);
		}
		catch (const std::runtime_error &e)
		{
			std::cerr << e.what();
			_clients2rm.push_back(read_clients[i]);
			continue ;
		}
		if (stream == "")
			_clients2rm.push_back(read_clients[i]);
		else
		{
			pos = findcrfl(stream);
			while (pos != std::string::npos)
			{
				_clients[read_clients[i]].setStream(stream.substr(0, pos));
				std::cout << "Complete message received: " << _clients[read_clients[i]].getStream() << std::endl;
				// parse and put message object within client at _clients[read_clients[i]]
				stream.erase(0, pos + 2);
				pos = findcrfl(stream);
			}
			_clients[read_clients[i]].setStream(stream);
		}
	}
}

void	Server::acceptClients()
{
	Client			client;
	sockaddr_in		addr;
	socklen_t		len = sizeof(addr);
	int				clientfd;

	try
	{	
		while (true)
		{
			clientfd = accept(_listening_socket.getSocketFd(), (struct sockaddr*)&addr, &len);
			if (clientfd < 0)
			{
				if (errno == EWOULDBLOCK || errno == EAGAIN)
					break ;
				throw std::runtime_error("[ERROR] connecting client: accept() failed.");
			}
			std::cout << "\033[32m✔\033[0m New client over IP: " << inet_ntoa(addr.sin_addr) 
				<< " port: " << ntohs(addr.sin_port) 
				<< " fd: " << clientfd << std::endl;
			_event_manager.addClient(clientfd);
			_clients[clientfd].setPort(_listening_socket.getPort());
		}
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << e.what() << '\n';
	}
}

void	Server::initListeningSocket()
{
	try
	{
		_listening_socket = Socket(_port);
	}
	catch (std::runtime_error &e)
	{
		std::cerr << "Couldn't initialize server\n" << e.what();
		exit(1);
	}
}

void	Server::run(const int &port, const std::string &password)
{
	int	ready_fds = 0;
	
	_port = port;
	_password = password;
	initListeningSocket();
	_event_manager = EventManager();
	_event_manager.addListen(_listening_socket.getSocketFd());
	if (signal(SIGINT, signal_handler) == SIG_ERR)
	{
		std::cerr << "Failed to register SIGINT handler.\n";
		return ;
	}
	std::cout << "Waiting for connections..." << std::endl;
	while (stop_server == 0)
	{
		ready_fds = poll(_event_manager.pollfds(), _event_manager.size(), -1);
		if (ready_fds < 0)
		{
			if (errno == EINTR)
				continue ;
			std::cerr << "poll: " << strerror(errno);
			continue ;
		}
		acceptClients();
		readClients();
		removeClients();
	}
	std::cout << "\nSIGINT received. Initiating server shutdown..." << '\n';
}
