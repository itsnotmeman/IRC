#include "Utility.hpp"

// Tests if 2 strings are equal in a case-insentive manner
bool Utility::noCase(const std::string& s1, const std::string& s2)
{
	std::string str1 = s1;
	std::string str2 = s2;

	 if (!str1.empty() && str1[0] == '#') {
        str1.erase(0, 1);
    }
	 if (!str2.empty() && str2[0] == '#') {
        str2.erase(0, 1);
    }

	for (std::string::iterator it = str1.begin(); it != str1.end(); ++it)
		*it = std::tolower(static_cast<unsigned char>(*it));
	for (std::string::iterator it = str2.begin(); it != str2.end(); ++it)
		*it = std::tolower(static_cast<unsigned char>(*it));
	if (str1 == str2)
		return true;
	return false;
}

bool Utility::isValidNick(const std::string& nick)
{
	if (nick.empty() || nick.size() > 9)
		return false;
	if (!std::isalpha(nick[0]))
		return false;
	for (std::string::size_type i = 1; i < nick.size(); ++i) {
		if (!std::isalnum(nick[i]) && nick[i] != '_')
			return false;
	}
	return true;
}

bool Utility::isValidChannelName(const std::string& name) {
	if (name.size() < 2 || name.size() > 9)
		return false;
	if (name[0] != '#')
		return false;
	for (std::string::size_type i = 1; i < name.size(); ++i) {
		if (!std::isalnum(name[i]) && name[i] != '_')
			return false;
	}
	return true;
}

std::string Utility::itoa(int i) {
	std::stringstream ss;
	ss << i;
	return ss.str();
}
