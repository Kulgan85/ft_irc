#include "Server.hpp"

std::vector<std::string> Server::_splitString(std::string str, char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (getline(ss, token, delim))
	{
		if (token.size() == 0)
			token.push_back(' ');
		tokens.push_back(token);
	}
	return tokens;
}

void	Server::JOIN(const int &sender_fd) {(void)sender_fd;}

void	Server::PASS(const int &sender_fd) {(void)sender_fd;}

void	Server::PMSG(const int &sender_fd) {(void)sender_fd;}

void	Server::LEAVE(const int &sender_fd) {(void)sender_fd;}

void	Server::OP(const int &sender_fd) {(void)sender_fd;}

void	Server::NICK(const int &sender_fd) {(void)sender_fd;}

void	Server::USER(const int &sender_fd) {(void)sender_fd;}

void	Server::LIST(const int &sender_fd) {(void)sender_fd;}
