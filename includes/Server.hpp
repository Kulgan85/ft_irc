#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <iostream>
# include <poll.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include "Exceptions.hpp"

class Server
{
private:
	Server();
	Server(const Server &src);
	Server &operator=(const Server &rhs);

	int		_setSocket(std::string port);
	void	_clientInput(int pfds_index);
	void	_addClient(void);
	int		_addToPoll(int new_fd);
	void	_removeFromPoll(int pfds_index);
	void	_sendMessage(std::string message, int sender_fd);

	const std::string	_port;
	const std::string	_password;
	struct pollfd		*_pfds;
	int					_socket_fd;
	int					_pfd_count;
	int					_max_pfd_count;
public:
	Server(std::string port, std::string password);
	~Server();

	void	launch(void);
};

#endif