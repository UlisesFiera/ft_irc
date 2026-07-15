#ifndef EVENTMANAGER_HPP
# define EVENTMANAGER_HPP

# include <poll.h>
# include <iostream>
# include <map>
# include <unistd.h>

class EventManager 
{
	public:
		// constructors
		EventManager();

		// copy constructors
		EventManager(const EventManager &copyEventManager);

		// operator overrides
		EventManager &operator=(const EventManager &copyEventManager);

		// destructors
		~EventManager();

		// functions
		pollfd*					pollfds();
		size_t					size() const;
		std::vector<pollfd> 	vector();
		void					close(int fd);
		void					update(int fd, short event);
		void					clear(int fd, short event);
		void					addListen(int fd);
		void					addClient(int fd);
		void					addRFile(int fd);
		void					addWFile(int fd);
		bool					isListen(int fd) const;
		bool					isClient(int fd) const;
		bool					isOutput(int fd) const;
		bool					isInput(int fd) const;
		std::vector<int>		readableClients() const;
		std::vector<int>		readableFiles() const;
		std::vector<int>		writableFiles() const;
		std::vector<int>		writableClients() const;
		std::vector<int>		errorClients() const;

		// setters and getters
		std::vector<int>		readableListen() const;
	private:
		//fields
		enum event
		{
			LISTEN,
			CLIENT,
			RFILE,
			WFILE
		};
		std::vector<pollfd>		_pollfds;
		std::map<int, event>	_socketfd2event;

		//functions
		void					addEvent(const int fd, const short event);
};

#endif
