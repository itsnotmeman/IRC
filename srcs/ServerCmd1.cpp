#include "Server.hpp"

// After executing any of NICK, USER or PASS we check if registration criteria are met
void Server::checkRegistration(User* usr)
{
	if (!usr->isRegistered() && usr->checkRegistrationCriteria())
	{
		if (password != "" && usr->getConnectionPassword() != password)
		{
			sendNumericReply(usr, ERR_PASSWDMISMATCH, "PASS");
			return;
		}
		// Registration successful
		usr->setRegistered();
		sendNumericReply(usr, RPL_WELCOME, usr->userURL());
		sendNumericReply(usr, RPL_YOURHOST, hostname);
		sendNumericReply(usr, RPL_CREATED, creation_time);
		sendNumericReply(usr, RPL_MYINFO, hostname + " :+tl");
		sendNumericReply(usr, RPL_STATSCONN, Utility::itoa(getUserCount()));
		sendNumericReply(usr, RPL_LUSERCHANNELS, Utility::itoa(getChannelCount()));
	}
	return;
}

// PASS
void Server::irc_pass(std::vector<std::string> cmd, int socket)
{
	User* usr = getUserBySocket(socket);
	if (!usr)
		return;
	if (cmd.size() < 2)
	{
		sendNumericReply(usr, ERR_NEEDMOREPARAMS, "PASS");
		return; 
	}
	if (usr->isRegistered())
	{
		sendNumericReply(usr, ERR_ALREADYREGISTRED, "PASS");
		return; 
	}
	usr->setConnectionPassword(cmd[1]);
	checkRegistration(usr);
}

// NICK
void Server::irc_nick(std::vector<std::string> cmd, int socket)
{
	User* usr = getUserBySocket(socket);
	if (!usr)
		return;
	if (cmd.size() < 2)
	{
		sendNumericReply(usr, ERR_NONICKNAMEGIVEN, "NICK");
		return; 
	}
	if (!Utility::isValidNick(cmd[1]))
	{
		std::cout << "NICK :" << cmd[1] << ":";
		sendNumericReply(usr, ERR_ERRONEUSNICKNAME, cmd[1]);
		return;
	}
	if (getUserByNickname(cmd[1]) != NULL)
	{
		sendNumericReply(usr, ERR_NICKNAMEINUSE, cmd[1]);
		return;
	}
	usr->setNickname(cmd[1]);
	checkRegistration(usr);
}

// USER
void Server::irc_user(std::vector<std::string> cmd, int socket)
{
	User* usr = getUserBySocket(socket);
	if (!usr)
		return;
	if (usr->isRegistered())
	{
		sendNumericReply(usr, ERR_ALREADYREGISTRED, "USER");
		return; 
	}
	if (cmd.size() < 5)
	{
		sendNumericReply(usr, ERR_NEEDMOREPARAMS, "USER");
		return; 
	}
	// Args 2 and 3 have no official role in IRC so we discard them
	usr->setUsername(cmd[1]);
	usr->setRealname(cmd[4]);
	checkRegistration(usr);
}
	
	