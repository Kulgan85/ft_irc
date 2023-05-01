#include "SHA1.hpp"
#include <vector>

#define LeftShift(bits,word) (((word) << (bits)) | ((word) >> (32-(bits)))) 

std::bitset<160>	SHA1Hash(std::string message)
{
	unsigned long h0 = 0x67452301;
	unsigned long h1 = 0xefcdab89;
	unsigned long h2 = 0x98badcfe;
	unsigned long h3 = 0x10325476;
	unsigned long h4 = 0xc3d2e1f0;
	unsigned long long messageLength = message.length() * 8UL * sizeof(char);

	std::vector<unsigned char> byteArray = std::vector<unsigned char>(message.length());
	byteArray.assign(message.begin(), message.end());

	byteArray.push_back(1 << 7);
	int padding = (64 - (byteArray.size() % 64)) - 8;
	if (padding < 0)
		padding = 64 + padding;
	byteArray.insert(byteArray.end(), padding, 0);
	for (int i = 56; i >= 0; i -= 8)
	byteArray.push_back((char)(messageLength >> i));

	while (!byteArray.empty())
	{

		unsigned long	words[80];
		int 			i;

		for (i = 0; i < 16; i++)
		{
			words[i] = (byteArray[i * 4] << 24);
			words[i] |= (byteArray[i * 4 + 1] << 16);
			words[i] |= (byteArray[i * 4 + 2] << 8);
			words[i] |= (byteArray[i * 4 + 3]);
		}
		for (i = 16; i < 80; i++)
		{
			words[i] = LeftShift(1, words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16]);
		}
		unsigned long	a = h0, b = h1, c = h2, d = h3, e = h4;
		unsigned long	f, k, temp;
		for (i = 0; i < 80; i++)
		{
			if (i < 20)
			{
				f = (b & c) ^ ((~b) & d); k = 0x5a827999;
			}
			else if (i < 40)
			{
				f = b ^ c ^ d; k = 0x6ed9eba1;
			}
			else if (i < 60)
			{
				f = (b & c) ^ (b & d) ^ (c & d); k = 0x8f1bbcdc;
			}
			else
			{
				f = b ^ c ^ d; k = 0xca62c1d6;
			}
			temp = LeftShift(5, a) + f + e + k + words[i];
			e = d;
			d = c;
			c = LeftShift(30, b);
			b = a;
			a = temp;
		}

		h0 += a;
		h1 += b;
		h2 += c;
		h3 += d;
		h4 += e;
		byteArray.erase(byteArray.begin(), byteArray.begin() + 64);
	}
	std::bitset<160> returnValue;
	returnValue |= h0;
	returnValue <<= 32;
	returnValue |= h1;
	returnValue <<= 32;
	returnValue |= h2;
	returnValue <<= 32;
	returnValue |= h3;
	returnValue <<= 32;
	returnValue |= h0;
	return returnValue;
}