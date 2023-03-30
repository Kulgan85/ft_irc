#include "Server.hpp"

void	Server::_removeFromPoll(int pfds_index)
{
	close(this->_pfds[pfds_index].fd);
	this->_pfds[pfds_index] = this->_pfds[this->_pfd_count - 1];
	--this->_pfd_count;
}

int	Server::_addToPoll(int new_fd)
{
	if (this->_pfd_count == this->_max_pfd_count)
		return (-1);
	this->_pfds[this->_pfd_count].fd = new_fd;
	this->_pfds[this->_pfd_count].events = POLLIN;
	++this->_pfd_count;
	return (0);
}

void	Server::_addClient(void)
{
	struct sockaddr_storage	addr;
	socklen_t				addr_len;
	int						new_fd;

	addr_len = sizeof(addr);
	new_fd = accept(this->_socket_fd, (struct sockaddr *)&addr, &addr_len);
	if (new_fd == -1)
		std::cerr << "Error: accept()\n";
	else
	{
		if (Server::_addToPoll(new_fd) == -1)
			return ;
		if (send(new_fd, "Welcome to our IRC server\n", 26, MSG_DONTWAIT) == -1)
			std::cerr << "Error: send()\n";
		else
		{
			std::cout << "New connection from " << inet_ntoa(((struct sockaddr_in*)&addr)->sin_addr) << " on socket " << new_fd << std::endl;
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
	}
	else if (byte_count < 0)
	{
		std::cerr << "Error: recv()" << std::endl;
	}
	else
	{
		std::string	message(buf, strlen(buf));
		if (message.back() == '\n')
			message.pop_back();
		std::string	ret = "We have recieved your message! It was: ";
		ret.append(message);
		ret.push_back('\n');
		if (message.compare(this->_password) == 0)
			ret.append("Congratulations! You input the password!\n");
		if (send(sender_fd, ret.c_str(), ret.length(), MSG_DONTWAIT) == -1)
			std::cerr << "Error: send" << std::endl;
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
}

Server::~Server()
{
	delete [] this->_pfds;
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
					_addClient();
				else
					_clientInput(i);
			}
		}
	}
}