#ifndef SERVER_HPP
#define SERVER_HPP

#include <bitset>

class Server
{
	private:
		std::string			m_name;
		std::bitset<160U>	m_hashedPassword;
};

#endif