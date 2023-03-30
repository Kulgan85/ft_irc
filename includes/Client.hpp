#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class	Client
{
	private:
		Client();
		Client(const Client &src);
		Client &operator=(const Client &rhs);

		std::string	_message;
		std::string	_nickname;
		std::string	_username;
		bool		_is_operator;
		bool		_is_verified;
		int			_fd;

	public:
		Client(int fd);
		~Client();

		void	setUsername(std::string username);
		void	setNickname(std::string nickname);
		void	setIsOperator(bool is_operator);
		void	setIsVerified(bool is_verified);
		void	addToMessage(std::string to_add);
		void	clearMessage(void);
		const std::string	&getUsername(void) const;
		const std::string	&getNickname(void) const;
		const bool			&getIsOperator(void) const;
		const bool			&getIsVerified(void) const;
		const int			&getFd(void) const;
		std::string			getMessage(void);
		bool				messageHasNewline(void) const;
};

#endif