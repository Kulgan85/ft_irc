#include <iostream>

int	main(int ac, char** av)
{
	if (ac != 3)
	{
		std::cout << "Usage: " << av[0] << " <port> <password>\n"; return 1;
	}
}