#include "Client.hpp"

Client::Client(int fd) : _message(""), _nickname("*"), _username(""), _ip_address(""), _is_operator(false), _is_verified(false), _is_registered(false), _fd(fd) {}

Client::~Client() {}

void	Client::setUsername(std::string username)
{
	this->_username = username;
}

void	Client::setNickname(std::string nickname)
{
	this->_nickname = nickname;
}

void	Client::setIsVerified(bool is_verified)
{
	this->_is_verified = is_verified;
}

void	Client::setIsRegistered(bool is_registered)
{
	this->_is_registered = is_registered;
}

void	Client::setIPAddress(std::string ip_address)
{
	this->_ip_address = ip_address;
}

void	Client::setIsOperator(bool is_operator)
{
	this->_is_operator = is_operator;
}

const std::string	&Client::getNickname(void) const
{
	return (this->_nickname);
}

const std::string	&Client::getUsername(void) const
{
	return (this->_username);
}

const bool			&Client::getIsOperator(void) const
{
	return (this->_is_operator);
}

const bool			&Client::getIsVerified(void) const
{
	return (this->_is_verified);
}

const bool			&Client::getIsRegistered(void) const
{
	return (this->_is_registered);
}

const std::string	&Client::getIPAddress(void) const
{
	return (this->_ip_address);
}

const int			&Client::getFd(void) const
{
	return (this->_fd);
}

std::string			Client::getMessage(void)
{
	return (this->_message);
}

bool	Client::messageHasNewline(void) const
{
	if (this->_message.find('\n') != std::string::npos)
		return (true);
	else
		return (false);
}

void	Client::addToMessage(std::string to_add)
{
	this->_message.append(to_add);
}

void	Client::clearMessage(void)
{
	this->_message.clear();
}