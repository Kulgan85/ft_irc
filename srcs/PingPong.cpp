#include "../includes/Server.hpp"

void	Server::PING(const int &sender_fd)
{
	std::vector<std::string>	args = _splitString(this->_clients[sender_fd]->getMessage());
	
	if (args.size() == 1)
	{
		std::string toSend = ":ircserv 461 ";
		toSend.append(_clients[sender_fd]->getNickname());
		toSend.append(" :Not enough parameters\r\n");
		send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return;
	}
	std::string toSend = ":ircserv PONG ";
	toSend.append("ircserv ");
	toSend.append(args[1]);
	toSend.append("\r\n");
	send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
}

void	Server::PONG(const int &sender_fd)
{
	//Do nothing
	(void)sender_fd;
}