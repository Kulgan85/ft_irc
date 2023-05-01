#include "Server.hpp"

void	Server::PASS(const int &sender_fd)
{
	std::vector<std::string>	args = _splitString(this->_clients[sender_fd]->getMessage());
	if (args.empty() == true)
		return ;
	std::string	to_send;
	if (this->_clients[sender_fd]->getIsVerified() == true)
	{
		to_send = ":ircserv 462 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Unauthorized command (already input the password)\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (args.size() < 2)
	{
		to_send = ":ircserv 461 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" ");
		to_send.append(args[0]);
		to_send.append(" :Not enough parameters\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (this->_password == SHA1Hash(args[1]))
	{
		this->_clients[sender_fd]->setIsVerified(true);
		to_send = ":ircserv 300 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :That was the correct password, you are now verified\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	else
	{
		to_send = ":ircserv 464 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Incorrect password\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
}

void	Server::OPER(const int &sender_fd)
{
	std::string	to_send;
	Client*		client = _clients[sender_fd];
	if (client->getIsRegistered() == false)
	{
		to_send = ":ircserv 451 ";
		to_send.append(client->getNickname());
		to_send.append(" :You have not registered\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (client->getIsOperator() == true)
	{
		to_send = ":ircserv 300 ";
		to_send.append(client->getNickname());
		to_send.append(" :You are already an operator\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	std::vector<std::string>	args = _splitString(client->getMessage());
	if (args.empty() == true)
		return ;
	if (args.size() < 3)
	{
		to_send = ":ircserv 461 ";
		to_send.append(client->getNickname());
		to_send.append(" ");
		to_send.append(args[0]);
		to_send.append(" :Not enough parameters\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (args[1] != client->getNickname())
	{
		to_send = ":ircserv 491 ";
		to_send.append(client->getNickname());
		to_send.append(" :Invalid nickname, you must use your own\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (args[2] == this->_oper_password)
	{
		client->setIsOperator(true);
		to_send = ":ircserv 381 ";
		to_send.append(client->getNickname());
		to_send.append(" :You are now an IRC operator\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		to_send.clear();
		to_send = ":ircserv 221 ";
		to_send.append(client->getNickname());
		to_send.append(" +");
		if (client->getIsOperator() == true)
			to_send.push_back('o');
		to_send.append("\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	else
	{
		to_send = ":ircserv 464 ";
		to_send.append(client->getNickname());
		to_send.append(" :Password incorrect\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
}

void	Server::NICK(const int &sender_fd)
{
	std::string	to_send;
	Client*		client = _clients[sender_fd];
	if (client->getIsVerified() == false)
	{
		to_send = ":ircserv 451 ";
		to_send.append(client->getNickname());
		to_send.append(" :You have not input the password\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	std::vector<std::string>	args = _splitString(client->getMessage());
	if (args.empty() == true)
		return ;
	if (args.size() < 2)
	{
		to_send = ":ircserv 431 ";
		to_send.append(client->getNickname());
		to_send.append(" :No nickname given\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (!Server::_isValidNick(args[1]))
	{
		to_send = ":ircserv 432 ";
		to_send.append(client->getNickname());
		to_send.append(" ");
		to_send.append(args[1]);
		to_send.append(" :Erroneous nickname\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (std::find(this->_nicknames.begin(), this->_nicknames.end(), args[1]) != this->_nicknames.end())
	{
		to_send = ":ircserv 433 ";
		to_send.append(client->getNickname());
		to_send.append(" ");
		to_send.append(args[1]);
		to_send.append(" :Nickname is already in use\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	to_send = ":";
	if (find(this->_nicknames.begin(), this->_nicknames.end(), client->getNickname()) != this->_nicknames.end())
		this->_nicknames.erase(find(this->_nicknames.begin(), this->_nicknames.end(), client->getNickname()));
	to_send.append(client->getNickname());
	client->setNickname(args[1]);
	this->_nicknames.push_back(args[1]);
	to_send.append(" NICK ");
	to_send.append(args[1]);
	to_send.append("\r\n");
	for (std::map<int, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		send(it->first, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
	}
	to_send = ":";
	to_send.append(this->_name);
	to_send.append(" NICK change successful: ");
	to_send.append(args[1]);
	to_send.append("\r\n");
	send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
	if (client->getUsername().size() > 0 && client->getIsRegistered() == false)
	{
		client->setIsRegistered(true);
		Server::_sendWelcome(sender_fd);
	}
}

void	Server::USER(const int &sender_fd)
{
	std::string	to_send;
	if (this->_clients[sender_fd]->getIsVerified() == false)
	{
		to_send = ":ircserv 451 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :You have not input the password\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (this->_clients[sender_fd]->getUsername().size() > 0)
	{
		to_send = ":ircserv 462 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Unauthorized command (already registered)\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	std::vector<std::string>	args = _splitString(this->_clients[sender_fd]->getMessage());
	if (args.empty() == true)
		return ;
	if (args.size() < 4)
	{
		to_send = ":ircserv 461 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" ");
		to_send.append(args[0]);
		to_send.append(" :Not enough parameters\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	this->_clients[sender_fd]->setUsername(args[1]);
	if (this->_clients[sender_fd]->getNickname().compare("*") != 0 && this->_clients[sender_fd]->getIsRegistered() == false)
	{
		this->_clients[sender_fd]->setIsRegistered(true);
		Server::_sendWelcome(sender_fd);
	}
}

void	Server::QUIT(const int &sender_fd)
{
	std::vector<std::string>	args = _splitString(this->_clients[sender_fd]->getMessage());
	std::string	quit_message = ":";
	quit_message.append(this->_clients[sender_fd]->getNickname());
	quit_message.push_back('!');
	quit_message.append(this->_clients[sender_fd]->getUsername());
	quit_message.push_back('@');
	quit_message.append(this->_name);
	quit_message.push_back(' ');
	quit_message.append(args[0]);
	if (args.size() > 1)
	{
		quit_message.append(" :");
		quit_message.append(args[1]);
	}
	quit_message.append("\r\n");
	std::string	error_message = "ERROR :";
	error_message.append(this->_name);
	error_message.append(" you issued a QUIT command\r\n");
	send(sender_fd, error_message.c_str(), error_message.size(), MSG_DONTWAIT);

	int	pfds_index = this->_pfd_count - 1;

	while (this->_pfds[pfds_index].fd != sender_fd && pfds_index >= 0)
		--pfds_index;
	if (pfds_index < 0)
	{
		std::cerr << "ERROR: Something is very wrong" << std::endl;
	}
	close(sender_fd);
	Server::_removeFromPoll(pfds_index);
	Server::_removeClient(sender_fd);
	std::cout << "Socket " << sender_fd << " hung up" << std::endl;
	for (std::map<int, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		send(it->second->getFd(), quit_message.c_str(), quit_message.size(), MSG_DONTWAIT);
	}
}

void	Server::KILL(const int &sender_fd)
{
	std::string	to_send;

	if (!this->_clients[sender_fd]->getIsRegistered())
	{
		to_send = ":ircserv 451 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :You have not registered\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return;
	}
	if (this->_clients[sender_fd]->getIsOperator() == false)
	{
		to_send = ":ircserv 481 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Permission Denied- You're not an IRC operator\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	std::vector<std::string>	args = _splitString(this->_clients[sender_fd]->getMessage());
	if (args.size() < 3)
	{
		to_send = ":ircserv 461 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.push_back(' ');
		to_send.append(args[0]);
		to_send.append(" :Not enough parameters\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (args[1] == this->_clients[sender_fd]->getNickname())
	{
		to_send = ":ircserv 483 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :You cannot KILL yourself\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	for (std::map<int, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (it->second->getNickname() == args[1])
		{
			std::string	kill_message = ":";
			kill_message.append(this->_clients[sender_fd]->getNickname());
			kill_message.append(" KILL ");
			kill_message.append(args[1]);
			kill_message.append(" :");
			kill_message.append(args[2]);
			kill_message.append("\r\n");
			for (std::map<int, Client *>::iterator it2 = this->_clients.begin(); it2 != this->_clients.end(); it2++)
				send(it2->first, kill_message.c_str(), kill_message.size(), MSG_DONTWAIT);

			int	pfds_index = this->_pfd_count - 1;
			while (this->_pfds[pfds_index].fd != it->first && pfds_index >= 0)
				--pfds_index;
			close(it->first);
			Server::_removeFromPoll(pfds_index);
			Server::_removeClient(it->first);
			return ;
		}
	}
	to_send = ":ircserv 401 ";
	to_send.append(this->_clients[sender_fd]->getNickname());
	to_send.push_back(' ');
	to_send.append(args[1]);
	to_send.append(" :No such nick\r\n");
	send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
}

void	Server::DIE(const int &sender_fd)
{
	std::string	to_send;

	if (!this->_clients[sender_fd]->getIsRegistered())
	{
		to_send = ":ircserv 451 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :You have not registered\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return;
	}
	if (this->_clients[sender_fd]->getIsOperator() == false)
	{
		to_send = ":ircserv 481 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Permission Denied- You're not an IRC operator\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	std::string	kill_message = ":ircserv 002 :";
	kill_message.append(this->_clients[sender_fd]->getNickname());
	kill_message.append(" has told the server to DIE\r\n");
	for (std::map<int, Client *>::iterator it2 = this->_clients.begin(); it2 != this->_clients.end(); it2++)
		send(it2->first, kill_message.c_str(), kill_message.size(), MSG_DONTWAIT);
	throw (SHUTDOWN_EXCEPTION());
}
