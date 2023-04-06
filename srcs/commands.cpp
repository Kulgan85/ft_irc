#include "Server.hpp"

std::vector<std::string> Server::_splitString(std::string str)
{
	if (str.at(str.length() - 1) == '\n')
		str.erase(str.length() - 1);
	if (str.at(str.length() - 1) == '\r')
		str.erase(str.length() - 1);
	std::vector<std::string> tokens;
	std::string	buf;

	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		buf.clear();
		if (str[i] == ':')
		{
// This next line removes the ':' character from the start of the last parameter
			++i;
			while (i < str.size())
			{
				buf.push_back(str[i]);
				++i;
			}
			tokens.push_back(buf);
		}
		else if (str[i] != ' ')
		{
			while (str[i] != ' ' && i < str.size())
			{
				if (!(Server::_isValidChar(str[i])))
				{
					std::cout << "the thing is |" << str[i] << "|\n"; 
					tokens.clear();
					return (tokens);
				}
				buf.push_back(str[i]);
				++i;
			}
			tokens.push_back(buf);
		}
	}
	return (tokens);
}

void	Server::JOIN(const int &sender_fd) {(void)sender_fd;}

void	Server::PASS(const int &sender_fd)
{
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
	if (args.empty() == true)
		return ;
	std::string	to_send;
	if (this->_clients[sender_fd]->getIsVerified() == true)
	{
		to_send = ":ircserv 462 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Unauthorized command (already registered)\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
	}
	else if (args.size() < 2)
	{
		to_send = ":ircserv 461 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" ");
		to_send.append(args[0]);
		to_send.append(" :Not enough parameters\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
	}
	else
	{
		if (this->_password == SHA1Hash(args[1]))
		{
			this->_clients[sender_fd]->setIsVerified(true);
			to_send = ":That was the correct password, you are now verified\n";
			send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		}
		else
		{
			to_send = ":Incorrect password\n";
			send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		}
		
	}
	
}

void	Server::PMSG(const int &sender_fd) {(void)sender_fd;}

void	Server::LEAVE(const int &sender_fd) {(void)sender_fd;}

void	Server::OP(const int &sender_fd) {(void)sender_fd;}

void	Server::NICK(const int &sender_fd)
{
	if (this->_clients[sender_fd]->getIsVerified() == false)
		return ;
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
	if (args.empty() == true)
		return ;
	std::string	to_send;
	if (args.size() < 2)
	{
		to_send = ":ircserv 431 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :No nickname given\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (!Server::_isValidNick(args[1]))
	{
		to_send = ":ircserv 432 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" ");
		to_send.append(args[1]);
		to_send.append(" :Erroneous nickname\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (std::find(this->_nicknames.begin(), this->_nicknames.end(), args[1]) != this->_nicknames.end())
	{
		to_send = ":ircserv 433 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" ");
		to_send.append(args[1]);
		to_send.append(" :Nickname is already in use\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	this->_clients[sender_fd]->setNickname(args[1]);
	this->_nicknames.push_back(args[1]);
	if (this->_clients[sender_fd]->getUsername().size() > 0 && this->_clients[sender_fd]->getIsNamed() == false)
	{
		this->_clients[sender_fd]->setIsNamed(true);
		Server::_sendWelcome(sender_fd);
	}
}

void	Server::USER(const int &sender_fd)
{
	if (this->_clients[sender_fd]->getIsVerified() == false)
		return ;
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
	if (args.empty() == true)
		return ;
	std::string	to_send;
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
	if (this->_clients[sender_fd]->getUsername().size() > 0)
	{
		to_send = ":ircserv 462 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Unauthorized command (already registered)\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	this->_clients[sender_fd]->setUsername(args[1]);
	if (this->_clients[sender_fd]->getNickname().compare("*") != 0 && this->_clients[sender_fd]->getIsNamed() == false)
	{
		this->_clients[sender_fd]->setIsNamed(true);
		Server::_sendWelcome(sender_fd);
	}
}

void	Server::LIST(const int &sender_fd) {(void)sender_fd;}
