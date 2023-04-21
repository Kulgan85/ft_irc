#include "../includes/Server.hpp"

void	Server::JOIN(const int &sender_fd)
{
	Client*						client = _clients[sender_fd];
	std::vector<std::string>	user_input = _splitString(client->getMessage());
	std::string 				toSend = ":";

	if (!client->getIsVerified())
	{
		toSend.append("ircserv 451 ");
		toSend.append(client->getNickname());
		toSend.append(" :You have not registered\r\n");
		send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return;
	}
	if (user_input.size() <= 2)
	{
		toSend.append("ircserv 461 ");
		toSend.append(client->getNickname());
		toSend.append(" JOIN :Not enough parameters\r\n");
		send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return;
	}
	if (user_input.size() == 3 && user_input[2] == "0");
	{
		for (std::vector<Channel*>::reverse_iterator it = client->joined_channels.rbegin(); it != client->joined_channels.rend(); it = client->joined_channels.rbegin())
			(*it)->LeaveChannel(client, "Leaving all channels");
		return;
	}
	if (user_input[2][0] != '#' || user_input[2][0] != '&')
	{
		// I can't find any info on what to do in this situation
		// There's nothing stopping a client from sending this, but channels need to start with # or &
		std::cout << "Error: Channel name must start with a # or an &" << std::endl;
		return ; 
	}
	std::map<std::string,Channel*>::iterator it = _channels.find(user_input[2]);
	if (it == _channels.end())
		_createChannel(user_input[2]);
	Channel *channelptr = _channels.find(user_input[2])->second;
	channelptr->JoinChannel(_clients[sender_fd]);
}

//[2] = channel names
//[3] = reason
void	Server::PART(const int &sender_fd)
{	
	Client*						client = _clients[sender_fd];
	std::vector<std::string>	user_input = _splitString(client->getMessage());
	if (user_input[2].empty())
	{
		std::string toSend = ":ircserv 461 ";
		toSend.append(client->getNickname());
		toSend.append(" PART :Not enough parameters\r\n");
		send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return ;
	}
	std::vector<std::string> target_channels = getTargets(user_input[2]);
	for (int i = 0; i < target_channels.size(); i++)
	{
		std::map<std::string,Channel*>::iterator it = _channels.find(target_channels[i]);
		if (it != _channels.end())
			it->second->LeaveChannel(client, user_input[3]);
		else
		{
			std::string toSend = ":ircserv 403 ";
			toSend.append(client->getNickname());
			toSend.push_back(' ');
			toSend.append(target_channels[i]);
			toSend.append(" :No such channel\r\n");
			send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		}
	}
}

void	Server::NAMES(const int& sender_fd)
{
	Client*						client = _clients[sender_fd];
	std::vector<std::string>	args = _splitString(client->getMessage());
	std::string toSend = ":";

	if (args.size() <= 2)
		return;
	std::vector<std::string>	targets = getTargets(args[2]);
	for (size_t i = 0; i < targets.size(); i++)
	{
		Channel* channel;
		try
		{
			channel = _channels.at(targets[i]);
		}
		catch(const std::exception& e)
		{
			toSend = ":ircserv 366 ";
			toSend.append(client->getNickname());
			toSend.push_back(' ');
			toSend.append(targets[i]);
			toSend.append(" :End of NAMES list\r\n");
			send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
			continue;
		}
		channel->SendNames(client);
	}
}

void	Server::TOPIC(const int& sender_fd)
{
	Client*						client = _clients[sender_fd];
	std::vector<std::string>	args = _splitString(client->getMessage());
	std::string toSend = ":";
	if (args.size() < 3)
	{
		toSend.append("ircserv 461 ");
		toSend.append(client->getNickname());
		toSend.append(" TOPIC :Not enough parameters\r\n");
		send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return;
	}
	Channel*	channel;
	try
	{
		channel = _channels.at(args[2]);
	}
	catch(const std::exception& e)
	{
		toSend.append("ircserv 403 ");
		toSend.append(client->getNickname());
		toSend.push_back(' ');
		toSend.append(args[2]);
		toSend.append(" :No such channel\r\n");
		send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return;
	}
	if (!channel->ClientIsInChannel(client))
	{
		toSend.append("ircserv 442 ");
		toSend.append(client->getNickname());
		toSend.push_back(' ');
		toSend.append(args[2]);
		toSend.append(" :You're not on that channel\r\n");
		send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return;
	}
	if (args.size() == 3)
	{
		std::string topic = channel->GetTopic();
		if (topic.empty())
			{ toSend.append("ircserv 331 "); topic = "No topic is set"; }
		else
			toSend.append("ircserv 332 ");
		toSend.append(client->getNickname());
		toSend.push_back(' ');
		toSend.append(args[2]);
		toSend.append(" :");
		toSend.append(topic);
		toSend.append("\r\n");
		send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return;
	}
	//	IDK if I even want channel operators
	//	if (!channel->IsChannelOperator(client))
	//		{ return "fuk off"; }
	channel->ChangeTopic(client, args[3]);
}

void	Server::LIST(const int &sender_fd)
{
	Client*						client = _clients[sender_fd];
	std::vector<std::string>	user_input = _splitString(client->getMessage());
	std::string 				toSend = ":";

	if (!client->getIsVerified())
	{
		toSend.append("ircserv 451 ");
		toSend.append(client->getNickname());
		toSend.append(" :You have not registered\r\n");
		send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return;
	}
	toSend.append("ircserv 321 ");
	toSend.append(client->getNickname());
	toSend.append(" Channel :Users Topic\r\n");
	send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	std::vector<std::string> targetChannels;
	if (user_input.size() > 2)
	{
		targetChannels = getTargets(user_input[2]);
	}
	else
	{
		for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
			targetChannels.push_back(it->second->GetName());
	}
	Channel* channel;
	for (int i = 0; i < targetChannels.size(); i++)
	{
		try
		{
			channel = _channels.at(targetChannels[i]);
		}
		catch(const std::exception& e)
		{
			continue;
		}
		toSend = ":ircserv 322 ";
		toSend.append(client->getNickname());
		toSend.push_back(' ');
		toSend.append(channel->GetName());
		toSend.push_back(' ');
		{
			//Definitely a better way to get a string from an int, it's currently 11:30PM tho and I'm tired
			std::stringstream stream;
			stream << channel->GetClientCount();
			std::string s;
			stream >> s;
			toSend.append(s);
		}
		toSend.append(" :");
		toSend.append(channel->GetTopic());
		toSend.append("\r\n");
		send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	}
	toSend = ":ircserv 323 ";
	toSend.append(client->getNickname());
	toSend.append(" :End of /LIST\r\n");
	send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
}

void	Server::KICK(const int& sender_fd)
{

}