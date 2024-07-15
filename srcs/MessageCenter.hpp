#ifndef MESSAGECENTER_HPP
#define MESSAGECENTER_HPP

#include <string>
#include <sstream>
#include <map>
#include <string>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <ctime>
#include <iomanip>

// Numeric reply codes as per IRC RFC
#define RPL_WELCOME 			"001"
#define RPL_YOURHOST 			"002"
#define RPL_CREATED 			"003"
#define RPL_MYINFO	 			"004"
#define RPL_STATSCONN			"250"
#define RPL_LUSERCHANNELS		"254"
#define RPL_LIST				"322"
#define RPL_LISTEND				"323"
#define RPL_CHANNELMODEIS		"324"
#define RPL_NOTOPIC				"331"
#define RPL_INVITING			"341"
#define RPL_TOPIC				"332"
#define RPL_NAMREPLY			"353"
#define RPL_ENDOFNAMES			"366"
#define ERR_NOSUCHNICK 			"401"
#define ERR_NOSUCHCHANNEL 		"403"
#define ERR_NORECIPIENT 		"411"
#define ERR_NONICKNAMEGIVEN		"431"
#define ERR_ERRONEUSNICKNAME 	"432"
#define ERR_NICKNAMEINUSE		"433"
#define ERR_USERNOTINCHANNEL	"441"
#define ERR_NOTONCHANNEL		"442"
#define ERR_USERONCHANNEL		"443"
#define ERR_NOTREGISTERED		"451"
#define ERR_NEEDMOREPARAMS 		"461"
#define ERR_ALREADYREGISTRED	"462"
#define ERR_PASSWDMISMATCH 		"464"
#define ERR_KEYSET		 		"467"
#define ERR_CHANNELISFULL		"471"
#define ERR_UNKNOWNMODE			"472"
#define ERR_INVITEONLYCHAN      "473"
#define ERR_BADCHANNELKEY		"475"
#define ERR_NOPRIVILEGES      	"481"
#define ERR_CHANOPRIVSNEEDED	"482"

#include "User.hpp"

class MessageCenter {

	private:
		std::string hostname;  // TO DELETE ???
    	std::map<std::string, std::string> numericReplies; // map of messages initialized by default constructor

	public:
		MessageCenter();
		
		std::string getHostName() const;  // TO DELETE ???
		std::string getTimeStamp() const;
		std::string getNumReply(std::string code);

};

#endif
