#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"

class Channel
{
	public:
	int	JoinChannel(Client* toJoin);
	int	LeaveChannel(Client* toLeave);

	private:
	std::string 			_name;
	std::vector<Client*>	_clients;
	std::vector<std::string>	_operatorIPs;
	std::vector<std::string>	_bannedIPs;

};

#endif