#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <vector>
#include <iostream>

#include "User.hpp"

class Utility {

	public:
		static bool noCase(const std::string& s1, const std::string& s2);
		static bool isValidNick(const std::string&);
		static bool isValidChannelName(const std::string&);
		static std::string itoa(int);
		};

#endif