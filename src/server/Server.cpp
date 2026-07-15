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
				std::string raw_command = stream.substr(0, pos);
				Message parsed_msg(raw_command);
				
				std::cout << "[DEBUG] Comando extraído: [" << parsed_msg.getCommand() << "]" << std::endl;
				executeCommand(read_clients[i], parsed_msg);
				
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

void Server::executeCommand(int client_fd, const Message &msg)
{
	std::string cmd = msg.getCommand();

	if (cmd == "NICK")
		std::cout << "-> Executing NICK for FD " << client_fd << std::endl;
	else if (cmd == "USER")
		std::cout << "-> Executing USER for FD " << client_fd << std::endl;
	else if (cmd == "JOIN")
		std::cout << "-> Executing JOIN for FD " << client_fd << std::endl;
	else if (cmd == "PRIVMSG")
		std::cout << "-> Executing PRIVMSG for FD " << client_fd << std::endl;
	else
		std::cout << "\033[33m[WARNING]\033[0m Unknown Command Received: " << cmd << std::endl;
}

/*
void Server::executeCommand(int client_fd, const Message &msg)
{
	std::string cmd = msg.getCommand();

	if (cmd == "NICK")
		handleNick(client_fd, msg);
	else if (cmd == "USER")
		handleUser(client_fd, msg);
	else if (cmd == "JOIN")
		handleJoin(client_fd, msg);
	else if (cmd == "PRIVMSG")
		handlePrivmsg(client_fd, msg);
	else
	{
		std::cout << "\033[33m[WARNING]\033[0m Comando desconocido recibido: " << cmd << std::endl;
	}
}

void Server::handleNick(int client_fd, const Message &msg)
{
	std::cout << "-> Ejecutando NICK para FD " << client_fd << std::endl;
	// TODO: Lógica para validar y guardar el Nickname
}

void Server::handleUser(int client_fd, const Message &msg)
{
	std::cout << "-> Ejecutando USER para FD " << client_fd << std::endl;
	// TODO: Lógica para registrar el Username
}

void Server::handleJoin(int client_fd, const Message &msg)
{
	std::cout << "-> Ejecutando JOIN para FD " << client_fd << std::endl;
	// TODO: Lógica para unirse a un canal
}

void Server::handlePrivmsg(int client_fd, const Message &msg)
{
	std::cout << "-> Ejecutando PRIVMSG para FD " << client_fd << std::endl;
	// TODO: Lógica para enviar mensajes a canales o usuarios
}
*/