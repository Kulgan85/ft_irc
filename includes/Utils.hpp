#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <deque>
#include <iostream>

std::deque<std::string>		_splitMessages(std::string message);
std::vector<std::string>	_splitString(std::string str);
bool						_isValidChar(char c);
std::vector<std::string>	getTargets(std::string commaList);

#endif