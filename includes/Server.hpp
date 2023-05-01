#ifndef SERVER_HPP
# define SERVER_HPP

class Channel;
class Client;

# include <string>
# include <map>
# include <vector>
# include <deque>
# include <algorithm>
# include <iostream>
# include <sstream>
# include <cstring>
# include <ctime>
# include <csignal>
# include <poll.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <utility>
# include <cctype>
# include "Channel.hpp"
# include "Client.hpp"
# include "Exceptions.hpp"
# include "SHA1.hpp"
# include "Utils.hpp"

class Server
{
	private:
		Server();
		Server(const Server &src);
		Server &operator=(const Server &rhs);

		int		_setSocket(std::string port);
		void	_clientInput(int pfds_index);
		void	_addClient(Client *new_client);
		void	_removeClient(int client_fd);
		void	_newClient(void);
		void	_addToPoll(int new_fd);
		void	_removeFromPoll(int pfds_index);
		void	_sendMessage(std::string message, int sender_fd);
		void	_useMessage(int sender_fd);
		void	_sendWelcome(int sender_fd);
		void	_runCommands(int sender_fd);
		bool	_isValidNick(std::string str);
		void	_createChannel(std::string name);
		void	_destroyEmptyChannels(void);
		void	_destroyChannel(std::string name);
		void	_destroyChannel(Channel* channel);
		void	_sendMessage(int sender_fd, bool silent = false);

		void	PASS(const int &sender_fd);
		void	NICK(const int &sender_fd);
		void	USER(const int &sender_fd);
		void	PMSG(const int &sender_fd);
		void	NOTICE(const int &sender_fd);
		void	JOIN(const int &sender_fd);
		void	PART(const int &sender_fd);
		void	TOPIC(const int &sender_fd);
		void	NAMES(const int &sender_fd);
		void	LIST(const int &sender_fd);
		void	KICK(const int& sender_fd);
		void	OPER(const int &sender_fd);
		void	QUIT(const int &sender_fd);
		void	KILL(const int &sender_fd);
		void	DIE(const int &sender_fd);
		void	PING(const int &sender_fd);
		void	PONG(const int &sender_fd);

		const std::string		_name;
		const std::string		_port;
		const std::bitset<160>	_password;
		const std::string		_oper_password;
		std::string				_start_time;
		struct pollfd			*_pfds;
		int						_socket_fd;
		int						_pfd_count;
		int						_max_pfd_count;
		std::vector<std::string>	_nicknames;
		std::map<int, Client *>	_clients;
		std::map<std::string, void (Server::*)(const int &)>	_commands;
		std::map<std::string, Channel*>		_channels;

	public:
		Server(std::string port, std::string password);
		~Server();

		void	launch(void);
};

#endif