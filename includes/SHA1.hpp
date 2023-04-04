#ifndef SHA1_HPP
#define SHA1_HPP

#include <bitset>

//#include <iostream>

std::bitset<160>	SHA1Hash(std::string message);

// std::string	BitsetToHex(std::bitset<160> set)
// {
// 	std::string str;
// 	std::string chars = std::string("0123456789abcdef");
// 	char c;
// 	str.reserve(42UL);
// 	str.append("0x");
// 	for (int i = 0; i < 40; i++)
// 	{
// 		c = 0;
// 		if (set.test(i * 4))
// 			c += 8;
// 		if (set.test(i * 4 + 1))
// 			c += 4;
// 		if (set.test(i * 4 + 2))
// 			c += 2;
// 		if (set.test(i * 4 + 3))
// 			c += 1;
// 		str.append(1, chars[c]);
// 	}
// 	return str;
// }

// int main()
// {
// 	std::bitset<160> set = SHA1Hash(std::string("Hello everyone and welcome to episode one of Jermacraft. This is gonna be c"));
// 	std::cout << "Hash = " << BitsetToHex(set) << '\n';
// }

#endif