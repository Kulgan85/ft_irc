#include "Server.hpp"

void	Server::_removeFromPoll(int pfds_index)
{
	close(this->_pfds[pfds_index].fd);
	this->_pfds[pfds_index] = this->_pfds[this->_pfd_count - 1];
	--this->_pfd_count;
}

void	Server::_addToPoll(int new_fd)
{
	this->_pfds[this->_pfd_count].fd = new_fd;
	this->_pfds[this->_pfd_count].events = POLLIN;
	++this->_pfd_count;
}

void	Server::_addClient(Client *new_client)
{
	this->_clients.insert(std::make_pair(new_client->getFd(), new_client));
}

void	Server::_removeClient(int client_fd)
{
	if (find(this->_nicknames.begin(), this->_nicknames.end(), this->_clients[client_fd]->getNickname()) != this->_nicknames.end())
		this->_nicknames.erase(find(this->_nicknames.begin(), this->_nicknames.end(), this->_clients[client_fd]->getNickname()));
	delete this->_clients[client_fd];
	this->_clients.erase(client_fd);
}

void	Server::_newClient(void)
{
	struct sockaddr_storage	addr;
	socklen_t				addr_len;
	int						new_fd;

	if (this->_pfd_count == this->_max_pfd_count)
		return ;
	addr_len = sizeof(addr);
	new_fd = accept(this->_socket_fd, (struct sockaddr *)&addr, &addr_len);
	if (new_fd == -1)
		std::cerr << "Error: accept()\n";
	else
	{
		Server::_addToPoll(new_fd);
		Server::_addClient(new Client(new_fd));
		std::cout << "New connection from " << inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr) << " on socket " << new_fd << std::endl;
		this->_clients[new_fd]->setIPAddress(inet_ntoa((((struct sockaddr_in*)&addr)->sin_addr)));
		send(new_fd, "*Welcome to ircserv. You must input a password before setting your username and nickname*\r\n", 92, MSG_DONTWAIT);
	}
}

void	Server::_runCommands(int sender_fd)
{
	std::deque<std::string>	messages = _splitMessages(this->_clients.at(sender_fd)->getMessage());
	while (messages.size() > 0)
	{
		std::cout << "message is |" << messages[0] << "|\n";
		this->_clients[sender_fd]->clearMessage();
		this->_clients[sender_fd]->addToMessage(messages[0]);
		if (messages[0].find_first_of('\r') != std::string::npos)
			messages[0].erase(messages[0].find_first_of('\r'), 1);
		if (messages[0].find_first_of(' ') != std::string::npos)
		{
			try
			{
				(this->*_commands.at(messages[0].substr(0, messages[0].find_first_of(' '))))(sender_fd);
			}
			catch(const std::exception& e)
			{
				std::string	to_send = ":ircserv 421 ";
				to_send.append(this->_clients[sender_fd]->getNickname());
				to_send.push_back(' ');
				to_send.append(messages[0].substr(0, messages[0].find_first_of(' ')));
				to_send.append(" :Unknown command\r\n");
				send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
			}
		}
		else
		{
			try
			{
				(this->*_commands.at(messages[0].substr(0, messages[0].find_first_of('\n'))))(sender_fd);
			}
			catch(const std::exception& e)
			{
				std::string	to_send = ":ircserv 421 ";
				to_send.append(this->_clients[sender_fd]->getNickname());
				to_send.push_back(' ');
				to_send.append(messages[0].substr(0, messages[0].find_first_of('\n')));
				to_send.append(" :Unknown command\r\n");
				send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
			}
		}
		messages.pop_front();
	}
}

bool	Server::_isValidNick(std::string str)
{
	if (str.size() > 9)
		return (false);
	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		if (isalnum(str[i]))
			continue ;
		if (str[i] == '-' || str[i] == '_')
			continue ;
		return (false);
	}
	return (true);
}

void	Server::_useMessage(int sender_fd)
{
	std::string message = this->_clients[sender_fd]->getMessage();
	if (message.at(message.length() - 1) != '\n')
		std::cerr << "Error: Missing newline" << std::endl;
	if (message.at(message.length() - 2) != '\r')
		std::cerr << "Error: Missing carriage return" << std::endl;
	Server::_runCommands(sender_fd);
/*	message.erase(message.length() - 1);
	for (std::vector<std::string>::iterator it = this->_clients[sender_fd]->joined_channels.begin(); it != this->_clients[sender_fd]->joined_channels.end(); it++)
	{
		for (std::vector<int>::iterator v_it = this->_channels.at(*it).begin(); v_it != this->_channels.at(*it).end(); v_it++)
		{
			send(*v_it, message.c_str(), message.size(), MSG_DONTWAIT);
		}
	}*/
}

void	Server::_sendWelcome(int sender_fd)
{
	std::string	to_send;

	std::cout << "Sending welcome\n";
	to_send = ":";
	to_send.append(this->_name);
	to_send.append(" 001 ");
	to_send.append(this->_clients[sender_fd]->getNickname());
	to_send.append(" :Welcome to the Internet Relay Network ");
	to_send.append(this->_clients[sender_fd]->getNickname());
	to_send.push_back('!');
	to_send.append(this->_clients[sender_fd]->getUsername());
	to_send.push_back('@');
	to_send.append(this->_clients[sender_fd]->getIPAddress());
	to_send.push_back('\r');
	to_send.push_back('\n');
	send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
	to_send.clear();
	to_send = ":";
	to_send.append(this->_name);
	to_send.append(" 002 ");
	to_send.append(this->_clients[sender_fd]->getNickname());
	to_send.append(" :Your host is ");
	to_send.append(this->_name);
	to_send.append(", running version 1\r\n");
	send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
	to_send.clear();
	to_send = ":";
	to_send.append(this->_name);
	to_send.append(" 003 ");
	to_send.append(this->_clients[sender_fd]->getNickname());
	to_send.append(" :This server was created ");
	to_send.append(this->_start_time);
	to_send.push_back('\r');
	to_send.push_back('\n');
	send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
	to_send.clear();
	to_send = ":";
	to_send.append(this->_name);
	to_send.append(" 004 ");
	to_send.append(this->_clients[sender_fd]->getNickname());
	to_send.append(" :");
	to_send.append(this->_name);
	to_send.append(" 1 \r\n");
	send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
}

void	Server::_clientInput(int pfds_index)
{
	char	buf[512];
	memset(buf, 0, 512);
	int		sender_fd = this->_pfds[pfds_index].fd;
	int		byte_count = recv(sender_fd, buf, sizeof(buf), MSG_DONTWAIT);

	if (byte_count == 0)
	{
		std::cout << "Socket " << sender_fd << " hung up" << std::endl;
		Server::_removeFromPoll(pfds_index);
		Server::_removeClient(sender_fd);
	}
	else if (byte_count < 0)
	{
		std::cerr << "Error: recv()" << std::endl;
	}
	else
	{
		std::string	message(buf, strlen(buf));
		this->_clients[sender_fd]->addToMessage(message);
		if (this->_clients[sender_fd]->getMessage().at(this->_clients[sender_fd]->getMessage().length() - 1) != '\n')
			return ;
		if (!(message.compare("\r\n") == 0 || message.compare("\n") == 0))
			Server::_useMessage(sender_fd);
		try
		{
			this->_clients.at(sender_fd)->clearMessage();
		}
		catch (const std::exception &e)
		{}
	}
}

int	Server::_setSocket(std::string port)
{
	struct addrinfo	hints;
	struct addrinfo	*res, *it;
	int				socket_fd;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = getprotobyname("TCP")->p_proto;

	if (getaddrinfo("0.0.0.0", port.c_str(), &hints, &res) != 0)
		throw (GAI_EXCEPTION());
	for (it = res; it != NULL; it = it->ai_next)
	{
		socket_fd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
		if (socket_fd < 0)
		{
			std::cerr << "Socket error\n";
			continue ;
		}
		int	opt_val = 1;
		setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
		if (bind(socket_fd, it->ai_addr, it->ai_addrlen) < 0)
		{
			close(socket_fd);
			std::cerr << "Bind error\n";
			continue ;
		}
		break ;
	}
	freeaddrinfo(res);

	if (it == NULL)
		throw (BIND_EXCEPTION());
	if (listen(socket_fd, 20))
		throw (LISTEN_EXCEPTION());
	return (socket_fd);
}

Server::Server(std::string port, std::string password) : _name("ircserv"), _port(port), _password(SHA1Hash(password)), _oper_password("password")
{
	time_t	rawtime;
	time(&rawtime);
	this->_start_time = asctime(localtime(&rawtime));
	this->_start_time.erase(this->_start_time.end() - 1);
	this->_socket_fd = Server::_setSocket(this->_port);
	this->_max_pfd_count = 20;
	this->_pfds = new struct pollfd[this->_max_pfd_count];
	this->_pfds[0].fd = this->_socket_fd;
	this->_pfds[0].events = POLLIN;
	this->_pfd_count = 1;
	this->_commands["PASS"] = &Server::PASS;
	this->_commands["NICK"] = &Server::NICK;
	this->_commands["USER"] = &Server::USER;
	this->_commands["PING"] = &Server::PING;
	this->_commands["PONG"] = &Server::PONG;
	this->_commands["OPER"] = &Server::OPER;
	this->_commands["QUIT"] = &Server::QUIT;
	//this->_commands["ERROR"] = &Server::ERROR;

	this->_commands["JOIN"] = &Server::JOIN;
	this->_commands["PART"] = &Server::PART;
	this->_commands["TOPIC"] = &Server::TOPIC;
	this->_commands["NAMES"] = &Server::NAMES;
	this->_commands["LIST"] = &Server::LIST;
	this->_commands["KICK"] = &Server::KICK;

	//this->_commands["MOTD"] = &Server::MOTD;
	//this->_commands["HELP"] = &Server::HELP;
	//this->_commands["INFO"] = &Server::INFO;
	//this->_commands["MODE"] = &Server::MODE;

	this->_commands["PRIVMSG"] = &Server::PMSG;
	this->_commands["NOTICE"] = &Server::NOTICE;

	//this->_commands["WHO"] = &Server::WHO;
	//this->_commands["WHOIS"] = &Server::WHOIS;
	//this->_commands["WHOWAS"] = &Server::WHOWAS;

	this->_commands["KILL"] = &Server::KILL;
}

Server::~Server()
{
	delete [] this->_pfds;
	for (std::map<int, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
		delete it->second;
	this->_clients.clear();
}

void	Server::launch()
{
	std::cout << "Server started on port " << _port << '\n';
	while (1)
	{
		if (poll(this->_pfds, this->_pfd_count, -1) == -1)
			throw (POLL_EXCEPTION());
		for (int i = 0; i < this->_pfd_count; i++)
		{
			if (this->_pfds[i].revents & POLLIN)
			{
				if (this->_pfds[i].fd == this->_socket_fd)
					_newClient();
				else
					_clientInput(i);
			}
		}
	}
}

void	Server::_createChannel(std::string name)
{
	Channel* newChan = new Channel(name);
	_channels.insert(std::make_pair(name, newChan));
}

void	Server::_destroyChannel(std::string name)
{
	Channel* chanPtr = _channels.find(name)->second;
	_destroyChannel(chanPtr);
}

void	Server::_destroyChannel(Channel* channel)
{
	(void)channel;
	// Should a channel be able to be destroyed if there are clients connected? 
	// Yes, an IRC server is technically able to destroy a channel even if there are users in it. 

	// When does a channel even get destroyed? Theres no command to. Is this only when the server closes?
	// When server closes but also if a user wants to

	// Should this function even exist?
	// I believe so
}