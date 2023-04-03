#include "Server.hpp"

std::vector<std::string> Server::_splitString(std::string str, char delim)
{
	if (str.at(str.length() - 1) == '\n')
		str.erase(str.length() - 1);
	if (str.at(str.length() - 1) == '\r')
		str.erase(str.length() - 1);
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (getline(ss, token, delim))
	{
		if (token.size() != 0)
			tokens.push_back(token);
	}
	return tokens;
}

void	Server::JOIN(const int &sender_fd) {(void)sender_fd;}

void	Server::PASS(const int &sender_fd)
{
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage(), ' ');
	(void)args;
}

void	Server::PMSG(const int &sender_fd) {(void)sender_fd;}

void	Server::LEAVE(const int &sender_fd) {(void)sender_fd;}

void	Server::OP(const int &sender_fd) {(void)sender_fd;}

void	Server::NICK(const int &sender_fd)
{
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage(), ' ');

	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++)
	{
		std::cout << "|" << *it << "|" << std::endl;
	}
}

void	Server::USER(const int &sender_fd) {(void)sender_fd;}

void	Server::LIST(const int &sender_fd) {(void)sender_fd;}
