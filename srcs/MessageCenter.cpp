#include "MessageCenter.hpp"

MessageCenter::MessageCenter() {
	hostname = getHostName();  // TO DELETE ???
	
    numericReplies["001"] = ":Welcome to IRC <P> !";
	numericReplies["002"] = ":Your host is <P>";
	numericReplies["003"] = ":This server was created <P>";
    numericReplies["004"] = ":<P>";
    numericReplies["250"] = ":Number of connections: <P>";
    numericReplies["254"] = ":Number of channels: <P>";
    numericReplies["322"] = "<P>";
    numericReplies["323"] = ":End of LIST";
    numericReplies["331"] = "<P> :No topic is set";
    numericReplies["332"] = "<P>";
    numericReplies["324"] = "<P>";
    numericReplies["341"] = "<P>";
    numericReplies["353"] = "<P>";
    numericReplies["366"] = "<P> :End of /NAMES list";
	numericReplies["401"] = "<P> :No such nick/channel";
    numericReplies["403"] = "<P> :No such channel";
    numericReplies["411"] = ":No recipient given (<P>)";
    numericReplies["431"] = "<P> :No nickname given";
    numericReplies["441"] = "<P> :They aren't on that channel";
    numericReplies["442"] = "<P> :You're not on that channel";
    numericReplies["443"] = "<P> :is already on channel";
	numericReplies["451"] = "<P> :You have not registered";
	numericReplies["461"] = "<P> :Not enough parameters";
	numericReplies["464"] = "<P> :Password incorrect";
    numericReplies["467"] = "<P> :Channel key already set";
	numericReplies["432"] = "<P> :Erroneus nickname";
	numericReplies["433"] = "<P> :Nickname is already in use";
	numericReplies["462"] = "<P> :You may not reregister";
    numericReplies["471"] = "<P> :Cannot join channel (+l)";
    numericReplies["472"] = "<P> :is unknown mode char to me";
    numericReplies["473"] = "<P> :Cannot join channel (+i)";
    numericReplies["475"] = "<P> :Cannot join channel (+k)";
    numericReplies["482"] = "<P> :You're not channel operator";
    
}

std::string MessageCenter::getHostName() const {    // TO DELETE ???
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        struct hostent* hostEntry = gethostbyname(hostname);
        if (hostEntry != NULL) {
            std::string hostnameString = hostEntry->h_name;
            return hostnameString;
        }
    }
    return 0;
}

std::string MessageCenter::getTimeStamp() const
{
    std::time_t currentTime = std::time(NULL);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S %b %d %Y", std::localtime(&currentTime));
    std::string str = buffer;
    return str;
}

std::string MessageCenter::getNumReply(std::string code)
{
	std::map<std::string, std::string>::iterator it = numericReplies.find(code);
    if (it == numericReplies.end())
        return "";
    return numericReplies[code];
}
