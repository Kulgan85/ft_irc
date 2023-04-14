#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"

class Channel
{
	public:
	Channel(std::string name);
	~Channel();
	int	JoinChannel(Client* toJoin);
	int	JoinChannel(int sender_fd);		//TO DO - ANNOUNCE USER JOINING
	int	LeaveChannel(Client* toLeave);
	int	LeaveChannel(int sender_fd, std::string reason);	//TO DO - ANNOUNCE USER LEAVING & REASON
	int	KickClient(Client* toKick);
	int	BanClient(Client* toBan);
	int	ChangeTopic(std::string newTopic);
	bool	ClientIsInChannel(Client* client);
	bool	ClientIsBanned(Client* client);

	private:
	Channel();

	const std::string 		_name;
	std::vector<Client*>	_clients;
	std::string				_topic;
	std::vector<std::string>	_operatorIPs;
	std::vector<std::string>	_bannedIPs;
};

#endif