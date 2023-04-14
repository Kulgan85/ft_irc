#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"

class Channel
{
	public:
	int	JoinChannel(Client* toJoin);
	int	JoinChannel(int sender_fd);		//TO DO - ANNOUNCE USER JOINING
	int	LeaveChannel(Client* toLeave);
	int	LeaveChannel(int sender_fd, std::string reason);	//TO DO - ANNOUNCE USER LEAVING & REASON

	private:
	std::string 			_name;
	std::vector<Client*>	_clients;
	std::vector<std::string>	_operatorIPs;
	std::vector<std::string>	_bannedIPs;

};

#endif