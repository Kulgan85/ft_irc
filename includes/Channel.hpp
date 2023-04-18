#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"

class Channel
{
	public:
	Channel(std::string name);
	~Channel();
	int	JoinChannel(Client* toJoin);
	int	LeaveChannel(Client* toLeave, std::string reason);
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