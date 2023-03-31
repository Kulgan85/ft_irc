#ifndef COMMANDS_HPP
#define	COMMANDS_HPP

#include <vector>
#include <string>
#include <iostream>
#include <map>

class Server;

class Client;

class Commands
{
	typedef void	(*f_cmd)(const Commands &c);

	public:
		Commands(const std::string &line, Client *sender);
		~Commands();

		static std::map<std::string, f_cmd>	cmds;
		void	exec(Server *server);

		Server	&get_server(void) const;
		Client	&get_client(void) const;

		const std::string				&get_prefix(void) const;
		const std::string				&get_cmd_name(void) const;
		const std::vector<std::string>	&get_arg(void) const;
		const std::string				&get_trailing(void) const;

	private:
		Server	*_server;
		Client	*_client;

		std::string					_prefix;
		std::string					_cmd_name;
		std::vector<std::string>	_arg;
		std::string					_trailing;

		Commands();
		Commands(const Commands &src);
		Commands	&operator=(const Commands &src);

	public:
		static void	PASS(const Commands &c);
		static void	NICK(const Commands &c);
		static void	USER(const Commands &c);
		static void	PMSG(const Commands &c);
		static void	JOIN(const Commands &c);
		static void	LEAVE(const Commands &c);
		static void	OP(const Commands &c);
		static void	LIST(const Commands &c);
};


#endif