#include "server/Server.hpp"
#include "message/Message.hpp"

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
	_host = other._host;
}

// Operator Overrides

Server& Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_port = other._port;
		_password = other._password;
		_listening_socket = other._listening_socket;
		_host = other._host;
	}
	return (*this);
}

// Destructors

Server::~Server() {}

sig_atomic_t	stop_server = 0;

void	Server::signal_handler(int signum)
{
	if (signum == SIGINT)
		stop_server = 1;
}

// Functions

void	Server::removeClients()
{
	const std::vector<int>		&error_clients = _event_manager.errorClients();

	for (size_t i = 0; i < error_clients.size(); i++)
	{
		if (std::find(_clients2rm.begin(), _clients2rm.end(), error_clients[i]) == _clients2rm.end())
			_clients2rm.push_back(error_clients[i]);
	}
	for (size_t i = 0; i < _clients2rm.size(); i++)
	{
		_clients.erase(_clients2rm[i]);
		_event_manager.close(_clients2rm[i]);
		std::cout << "\033[32m✔\033[0m Client " << _clients2rm[i] << " disconnected." << std::endl;
	}
	_clients2rm.clear();
}

void	Server::checkWritingDone()
{
	const std::vector<int>	&write_clients = _event_manager.writableClients();

	for (size_t i = 0; i < write_clients.size(); i++)
	{
		for (size_t j = _clients[write_clients[i]].getResponses().size(); j-- > 0;)
		{
			if (_clients[write_clients[i]].getResponses()[j].getBytesSent()
				== _clients[write_clients[i]].getResponses()[j].getResponse().size())
				_clients[write_clients[i]].getResponses().erase(_clients[write_clients[i]].getResponses().begin() + j);
		}
		if (_clients[write_clients[i]].getResponses().empty())
			_event_manager.clear(write_clients[i], POLLOUT);
	}
}

void	Server::respond(Response &response, int client_fd)
{
	std::string	rstream = response.getResponse();
	const char*	remainder = rstream.c_str() + response.getBytesSent();
	size_t		remainder_bytes = rstream.size() - response.getBytesSent();
	ssize_t		sent = send(client_fd, remainder, remainder_bytes, MSG_NOSIGNAL);

	if (sent < 0)
	{
		std::cerr << "Error writing to client\n";
		return ;
	}
	response.setBytesSent(response.getBytesSent() + sent);
}

void	Server::respondClients()
{
	const std::vector<int>	&write_clients = _event_manager.writableClients();

	for (size_t i = 0; i < write_clients.size(); i++)
	{
		for (size_t j = 0; j < _clients[write_clients[i]].getResponses().size(); j++)
			for (size_t z = 0; z < _clients[write_clients[i]].getResponses()[j].getTargets().size(); z++)
				respond(_clients[write_clients[i]].getResponses()[j], _clients[write_clients[i]].getResponses()[j].getTargets()[z]);
	}
}

void	Server::execute(Client &client, const Message &message, const commands &command)
{
	if (command == USER || command == PASS)
		return ;
	if (!client.isRegistered())
	{
		client.setResponse(Response(client, message, ERR_NOTREGISTERED));
		return ;
	}
	switch (message.getCommand())
	{
		case NICK:
			changeNick(client, message);
			break ;
		case JOIN:
			joinChannel(client, message);
			break ;
		case PRIVMSG:
			sendPrivmsg(client, message);
			break ;
		case KICK:
			break ;
		case INVITE:
			break ;
		case TOPIC:
			break ;
		case MODE:
			break ;
		case INVALID:
			client.setResponse(Response(client, message, ERR_UNKNOWNCOMMAND));
			break ;
		default:
			client.setResponse(Response(client, message, ERR_UNKNOWNCOMMAND));
			break ;
	}
}

void	Server::executeCommands()
{
	std::map<int, Client>::iterator	it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		for (size_t j = 0; j < it->second.getMessages().size(); j++)
			execute(it->second, it->second.getMessages()[j], it->second.getMessages()[j].getCommand());
		it->second.getMessages().clear();
	}
}

void	Server::registerClients()
{
	const std::vector<int>	&read_clients = _event_manager.readableClients();

	for (size_t i = 0; i < read_clients.size(); i++)
	{
		if (!_clients[read_clients[i]].isRegistered())
		{
			for (size_t j = 0; j < _clients[read_clients[i]].getMessages().size(); j++)
			{
				registerPass(_clients[read_clients[i]], _clients[read_clients[i]].getMessages()[j]);
				registerNick(_clients[read_clients[i]], _clients[read_clients[i]].getMessages()[j]);
				registerUser(_clients[read_clients[i]], _clients[read_clients[i]].getMessages()[j]);
				if (_clients[read_clients[i]].getPass() != "" && _clients[read_clients[i]].getUser() != "" && _clients[read_clients[i]].getNick() != "")
				{
					_clients[read_clients[i]].setRegistered(true);
					_clients[read_clients[i]].setResponse(Response(_clients[read_clients[i]], _clients[read_clients[i]].getMessages()[j], RPL_WELCOME));
				}
			}
		}
	}
}

std::string	Server::readStream(int client_fd)
{
	char		buffer[BUFFER_SIZE];
	ssize_t		bytes_read = read(client_fd, buffer, BUFFER_SIZE);
	std::string	stored_stream = _clients[client_fd].getStream();

	std::cout << "Stream received: " << std::endl;
	printcrlf(buffer);
	if (bytes_read > 0)
	{
		stored_stream.append(buffer, bytes_read);
		_clients[client_fd].setLastActivity(); 
		return (stored_stream);
	}
	else if (bytes_read == 0)
		return ("");
	else
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return ("");
		throw std::runtime_error("Read() failed");
	}
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
		catch (std::runtime_error &e)
		{
			std::cerr << e.what() << "\n";
			_clients2rm.push_back(read_clients[i]);
			continue ;
		}
		if (stream == "")
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				continue ;
			else
			{
				_clients2rm.push_back(read_clients[i]);
				continue ;
			}
		}
		else
		{
			pos = findcrfl(stream);
			while (pos != std::string::npos)
			{
				_event_manager.update(read_clients[i], POLLOUT);
				_clients[read_clients[i]].setMessage(stream.substr(0, pos));
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

	while (true)
	{
		clientfd = accept(_listening_socket.getSocketFd(), (struct sockaddr*)&addr, &len);
		if (clientfd < 0)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break ;
			std::cerr << "[ERROR] connecting client: accept() failed.\n";
			return ;
		}
		std::cout << "\033[32m✔\033[0m New client over IP: " << inet_ntoa(addr.sin_addr) 
					<< " port: " << ntohs(addr.sin_port) 
					<< " fd: " << clientfd << std::endl;
		_event_manager.addClient(clientfd);
		_clients[clientfd].setPort(_listening_socket.getPort());
		_clients[clientfd].setHost(_host);
	}
}

void	Server::initListeningSocket()
{
	try
	{
		_listening_socket = Socket(_port);
		_host = inet_ntoa(_listening_socket.getAddress().sin_addr);
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
		registerClients();
		executeCommands();
		respondClients();
		checkWritingDone();
		removeClients();
	}
	std::cout << "\nSIGINT received. Initiating server shutdown..." << '\n';
}
