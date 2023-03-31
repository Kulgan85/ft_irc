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
		if (send(new_fd, "Welcome to our IRC server\n", 26, MSG_DONTWAIT) == -1)
			std::cerr << "Error: send()\n";
		else
		{
			std::cout << "New connection from " << inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr) << " on socket " << new_fd << std::endl;
		}
	}
}

bool	Server::_isCommand(int sender_fd)
{
	std::string	message = this->_clients.at(sender_fd)->getMessage();
	if (message.find_first_of(' ') != std::string::npos)
	{
		try
		{
			this->_commands.at(message.substr(0, message.find_first_of(' ')))(sender_fd);
			return (true);
		}
		catch(const std::exception& e)
		{
			return (false);
		}
	}
	else
	{
		try
		{
			this->_commands.at(message.substr(0, message.find_first_of('\n')))(sender_fd);
			return (true);
		}
		catch(const std::exception& e)
		{
			return (false);
		}
	}
}

void	Server::_useMessage(int sender_fd)
{
/* Do A bunch of parsing and the run the command or send the message
	All content is currently a placeholder */
	std::string message = this->_clients[sender_fd]->getMessage();
	if (message.back() != '\n')
		std::cerr << "Error: This message should not have been allowed through" << std::endl;

	if (Server::_isCommand(sender_fd))
		return ;
	message.pop_back();
	for (std::vector<std::string>::iterator it = this->_clients[sender_fd]->joined_channels.begin(); it != this->_clients[sender_fd]->joined_channels.end(); it++)
	{
		for (std::vector<int>::iterator v_it = this->_channels.at(*it).begin(); v_it != this->_channels.at(*it).end(); v_it++)
		{
			send(*v_it, message.c_str(), message.size(), MSG_DONTWAIT);
		}
	}
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
		close(sender_fd);
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
		if (this->_clients[sender_fd]->getMessage().back() != '\n')
			return ;
		Server::_useMessage(sender_fd);
		this->_clients[sender_fd]->clearMessage();
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

Server::Server(std::string port, std::string password) : _port(port), _password(password)
{
	this->_socket_fd = Server::_setSocket(this->_port);
	this->_max_pfd_count = 20;
	this->_pfds = new struct pollfd[this->_max_pfd_count];
	this->_pfds[0].fd = this->_socket_fd;
	this->_pfds[0].events = POLLIN;
	this->_pfd_count = 1;
	this->_commands.insert(std::make_pair("PASS", &this->PASS));
	this->_commands.insert(std::make_pair("NICK", &this->NICK));
	this->_commands.insert(std::make_pair("USER", &this->USER));
	this->_commands.insert(std::make_pair("PMSG", &this->PMSG));
	this->_commands.insert(std::make_pair("JOIN", &this->JOIN));
	this->_commands.insert(std::make_pair("LEAVE", &this->LEAVE));
	this->_commands.insert(std::make_pair("OP", &this->OP));
	this->_commands.insert(std::make_pair("LIST", &this->LIST));
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