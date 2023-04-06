#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

class	Client
{
	private:
		Client();
		Client(const Client &src);
		Client &operator=(const Client &rhs);

		std::string	_message;
		std::string	_nickname;
		std::string	_username;
		std::string	_ip_address;
		bool		_is_operator;
		bool		_is_verified;
		bool		_is_named;
		int			_fd;

	public:
		Client(int fd);
		~Client();

		std::vector<std::string>	joined_channels;

		void	setUsername(std::string username);
		void	setNickname(std::string nickname);
		void	setIsOperator(bool is_operator);
		void	setIsVerified(bool is_verified);
		void	setIsNamed(bool is_named);
		void	setIPAddress(std::string ip_address);
		void	addToMessage(std::string to_add);
		void	clearMessage(void);
		const std::string	&getUsername(void) const;
		const std::string	&getNickname(void) const;
		const bool			&getIsOperator(void) const;
		const bool			&getIsVerified(void) const;
		const bool			&getIsNamed(void) const;
		const std::string	&getIPAddress(void) const;
		const int			&getFd(void) const;
		std::string			getMessage(void);
		bool				messageHasNewline(void) const;
};

#endif