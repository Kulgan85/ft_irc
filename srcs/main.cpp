#include "Server.hpp"

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	Server irc(argv[1], argv[2]);
	try
	{
		irc.launch();
	}
	catch(const SHUTDOWN_EXCEPTION& e)
	{
		(void)e;
		return (0);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}