#include "Client.hpp"

Client::Client(int fd) : _message(""), _nickname(""), _username(""), _is_operator(false), _is_verified(false), _fd(fd) {}

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