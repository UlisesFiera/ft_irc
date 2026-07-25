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

void	Server::removeFromChannels(Client &client)
{
	std::map<std::string, Channel*>::iterator	it;
	std::map<std::string, Channel*>::iterator	next;

	for (it = _channels.begin(); it != _channels.end(); it++)
	{
		for (size_t i = 0; i < client.getChannels().size(); i++)
		{
			if (it->first == client.getChannels()[i]->getName())
			{
				std::cout << "\033[35mircserv@asulgernan:\033[0mRemoving " << client.getNick() << " from " << it->first << std::endl;
				createStreamingResponse(client, Message("PART " + it->first), it->second->getNicks());
				it->second->removeMember(client);
				client.removeChannel(*it->second);
			}
		}
	}
	for (it = _channels.begin(); it != _channels.end();)
	{
		if (it->second->isEmpty())
		{
			next = it;
			next++;
			delete it->second; 
			_channels.erase(it); 
			it = next;
		}
		else
			it++;
	}
}

void	Server::channelCleanup()
{
	const std::vector<int>	&error_clients = _event_manager.errorClients();

	for (size_t i = 0; i < error_clients.size(); i++)
	{
		if (std::find(_clients2rm.begin(), _clients2rm.end(), error_clients[i]) == _clients2rm.end())
			_clients2rm.push_back(error_clients[i]);
	}
	for (size_t i = 0; i < _clients2rm.size(); i++)
		removeFromChannels(*_clients[_clients2rm[i]]);
}

void	Server::removeClients()
{
	const std::vector<int>		&error_clients = _event_manager.errorClients();

	for (size_t i = 0; i < error_clients.size(); i++)
	{
		if (std::find(_clients2rm.begin(), _clients2rm.end(), error_clients[i]) == _clients2rm.end())
			_clients2rm.push_back(error_clients[i]);
	}
	for (size_t i = 0; i < _clients2rm.size();)
	{
		delete _clients[_clients2rm[i]];
		_clients.erase(_clients2rm[i]);
		_event_manager.close(_clients2rm[i]);
		std::cout << "\033[35mircserv@asulgernan:\033[0mClient " << _clients2rm[i] << " disconnected." << std::endl;
		_clients2rm.erase(_clients2rm.begin() + i);
	}
}

void	Server::checkWritingDone()
{
	const std::vector<int>	&write_clients = _event_manager.writableClients();

	for (size_t i = 0; i < write_clients.size(); i++)
	{
		for (size_t j = _clients[write_clients[i]]->getResponses().size(); j-- > 0;)
		{
			if (_clients[write_clients[i]]->getResponses()[j].getBytesSent()
				== _clients[write_clients[i]]->getResponses()[j].getResponse().size())
				_clients[write_clients[i]]->getResponses().erase(_clients[write_clients[i]]->getResponses().begin() + j);
		}
		if (_clients[write_clients[i]]->getResponses().empty())
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
		std::cerr << "\033[35mircserv@asulgernan:\033[0mError writing to client " << client_fd << std::endl;
		return ;
	}
	if (sent > 0)
	{
		response.setBytesSent(response.getBytesSent() + sent);
		std::cout << "\033[35mircserv@asulgernan:\033[0mResponse stream sent to client " << client_fd << ":\n\033[33mclient@" << client_fd << ":\033[0m";
		printcrlf(remainder, sent);
	}
}

void	Server::respondClients()
{
	const std::vector<int>	&write_clients = _event_manager.writableClients();

	for (size_t i = 0; i < write_clients.size(); i++)
	{
		for (size_t j = 0; j < _clients[write_clients[i]]->getResponses().size(); j++)
			for (size_t z = 0; z < _clients[write_clients[i]]->getResponses()[j].getTargets().size(); z++)
				respond(_clients[write_clients[i]]->getResponses()[j], _clients[write_clients[i]]->getResponses()[j].getTargets()[z]);
	}
}

void	Server::execute(Client &client, const Message &message, const commands &command)
{
	std::cout << "\033[35mircserv@asulgernan:\033[0mExecuting command " << getCommandString(command) << " for client " << client.getFd() << std::endl;
	if (message.getCommand() == CAP)
	{
		createStreamingResponse(client, message, client.getFd());
		return ;
	}
	if (command == USER || command == PASS)
		return ;
	if (command == NICK && !client.isRegistered())
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
			kickChannel(client, message);
			break ;
		case INVITE:
			channelInvite(client, message);
			break ;
		case TOPIC:
			channelTopic(client, message);
			break ;
		case MODE:
			channelMode(client, message);
			break ;
		case PART:
			channelPart(client, message);
			break;
		case PONG:
			pong(client, message);
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
	std::map<int, Client*>::iterator	it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		for (size_t j = 0; j < it->second->getMessages().size(); j++)
			execute(*it->second, it->second->getMessages()[j], it->second->getMessages()[j].getCommand());
		it->second->getMessages().clear();
	}
}

void	Server::registerClients()
{
	const std::vector<int>	&read_clients = _event_manager.readableClients();

	for (size_t i = 0; i < read_clients.size(); i++)
	{
		if (!_clients[read_clients[i]]->isRegistered())
		{
			for (size_t j = 0; j < _clients[read_clients[i]]->getMessages().size(); j++)
			{
				if (_clients[read_clients[i]]->getMessages()[j].getCommand() == CAP)
					continue ;
				if (_clients[read_clients[i]]->getMessages()[j].getCommand() != NICK
					&& _clients[read_clients[i]]->getMessages()[j].getCommand() != USER
					&& _clients[read_clients[i]]->getMessages()[j].getCommand() != PASS)
					continue ;
				if (_clients[read_clients[i]]->getMessages()[j].getCommand() == PASS)
					registerPass(*_clients[read_clients[i]], _clients[read_clients[i]]->getMessages()[j]);
				else if (_clients[read_clients[i]]->getMessages()[j].getCommand() == NICK)
					registerNick(*_clients[read_clients[i]], _clients[read_clients[i]]->getMessages()[j]);
				else if (_clients[read_clients[i]]->getMessages()[j].getCommand() == USER)
					registerUser(*_clients[read_clients[i]], _clients[read_clients[i]]->getMessages()[j]);
				if (_clients[read_clients[i]]->getPass() != "" && _clients[read_clients[i]]->getUser() != "" && _clients[read_clients[i]]->getNick() != "")
				{
					_clients[read_clients[i]]->setRegistered(true);
					_clients[read_clients[i]]->setResponse(Response(*_clients[read_clients[i]], _clients[read_clients[i]]->getMessages()[j], RPL_WELCOME));
					_clients[read_clients[i]]->setResponse(Response(*_clients[read_clients[i]], _clients[read_clients[i]]->getMessages()[j], RPL_YOURHOST));
					_clients[read_clients[i]]->setResponse(Response(*_clients[read_clients[i]], _clients[read_clients[i]]->getMessages()[j], RPL_CREATED));
					_clients[read_clients[i]]->setResponse(Response(*_clients[read_clients[i]], _clients[read_clients[i]]->getMessages()[j], RPL_MYINFO));
					_clients[read_clients[i]]->setResponse(Response(*_clients[read_clients[i]], _clients[read_clients[i]]->getMessages()[j], RPL_ISUPPORT));
				}
			}
		}
	}
}

std::string	Server::readStream(int client_fd)
{
	char		buffer[BUFFER_SIZE];
	ssize_t		bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	std::string	stored_stream = _clients[client_fd]->getStream();

	if (bytes_read > 0)
	{
		if (bytes_read > STREAM_MAX)
		{
			std::cout << "\033[35mircserv@asulgernan:\033[0mStream from client " << client_fd << " rejected: exceeded stream size limit" << std::endl;
			return ("");
		}
		std::cout << "\033[35mircserv@asulgernan:\033[0mStream received from client " << client_fd << ":\n\033[33mclient@" << client_fd << ":\033[0m";
		printcrlf(buffer, bytes_read);
		stored_stream.append(buffer, bytes_read);
		_clients[client_fd]->setLastActivity(); 
		return (stored_stream);
	}
	else if (bytes_read == 0)
	{
		std::cout << "\033[35mircserv@asulgernan:\033[0mClient " << client_fd << " closed the connection" << std::endl;
		_clients2rm.push_back(client_fd);
		return ("");
	}
	else
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return ("");
		throw std::runtime_error("\033[35mircserv@asulgernan:\033[0mRead() failed");
	}
}

void	Server::readClients()
{
	const std::vector<int>		&read_clients = _event_manager.readableClients();
	std::string					stream;
	size_t						pos;

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
			continue ;
		else
		{
			pos = findcrfl(stream);
			while (pos != std::string::npos)
			{
				_event_manager.update(read_clients[i], POLLOUT);
				_clients[read_clients[i]]->setMessage(stream.substr(0, pos));
				stream.erase(0, pos + 2);
				pos = findcrfl(stream);
			}
			_clients[read_clients[i]]->setStream(stream);
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
			std::cerr << "\033[35mircserv@asulgernan:\033[0mError connecting client: accept() failed.\n";
			return ;
		}
		std::cout << "\033[35mircserv@asulgernan:\033[0mNew client over IP: " << inet_ntoa(addr.sin_addr) 
					<< " port: " << ntohs(addr.sin_port) 
					<< " fd: " << clientfd << std::endl;
		_clients[clientfd] = new Client();
		_event_manager.addClient(clientfd);
		_clients[clientfd]->setPort(_listening_socket.getPort());
		_clients[clientfd]->setHost(_host);
		_clients[clientfd]->setFd(clientfd);
		_clients[clientfd]->setAddr(inet_ntoa(addr.sin_addr));
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
		std::cerr << "\033[35mircserv@asulgernan:\033[0mCouldn't initialize server\n" << e.what();
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
		std::cerr << "\033[35mircserv@asulgernan:\033[0mFailed to register SIGINT handler.\n";
		return ;
	}
	std::cout << "\033[35mircserv@asulgernan:\033[0mWaiting for connections..." << std::endl;
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
		channelCleanup();
		respondClients();
		checkWritingDone();
		removeClients();
	}
	std::cout << "\n\033[35mircserv@asulgernan:\033[0mSIGINT received. Initiating server shutdown...";
}
