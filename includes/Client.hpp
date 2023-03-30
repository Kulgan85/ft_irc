#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class	Client
{
	private:
		Client();
		Client(const Client &src);
		Client &operator=(const Client &rhs);

		std::string	_nickname;
		std::string	_username;
		bool		_is_operator;
		bool		_is_verified;
		int			_fd;

	public:
		Client(int fd);
		~Client();
};

#endif