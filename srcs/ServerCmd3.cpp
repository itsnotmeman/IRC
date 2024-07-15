#include "Server.hpp"

// KICK - Parameters: <channel> <user> [<comment>] 
void Server::irc_kick(std::vector<std::string> cmd, int socket)
{
    User* usr = getUserBySocket(socket);
    
    if (!usr || !usr->isRegistered())
        return;
    if (cmd.size() < 3)
    {
        sendNumericReply(usr, ERR_NEEDMOREPARAMS, "KICK");
        return;
    }
    if (!getChannelByName(cmd[1]))
    {
        sendNumericReply(usr, ERR_NOSUCHCHANNEL, cmd[1]);
        return;
    }
	if (!getUserByNickname(cmd[2]))
    {
        return;
    }
    Channel* ch = getChannelByName(cmd[1]);
	User* target = getUserByNickname(cmd[2]);
    if (!ch->isOperator(usr) && usr != target)
	{
		sendNumericReply(usr, ERR_CHANOPRIVSNEEDED, ch->getName());
        return;
    }
	if (!ch->isInChannel(cmd[2]))
		return;
	std::string msg = cmd[1] + " " + cmd[2];
	if (cmd.size() == 4)
		msg += " :" + cmd[3];
	std::cout << cmd[2] << " was kicked from " << ch->getName() << std::endl;
    sendChanReply(usr, "KICK " + msg, cmd[1]);
	ch->removeUser(getUserByNickname(cmd[2]));
	deleteEmptyChannels();
}

// INVITE - Parameters: <nickname> <channel>
void Server::irc_invite(std::vector<std::string> cmd, int socket)
{
	User* usr = getUserBySocket(socket);
	
	if (!usr || !usr->isRegistered())
	{
		std::cout << "Unregistered user can't use INVITE" << std::endl;
		return;
	}
	if (cmd.size() < 3)
	{
		sendNumericReply(usr, ERR_NEEDMOREPARAMS, "INVITE");
		return;
	}
	if (!getChannelByName(cmd[2]))
	{
		std::cout << "Can't invite to channel " << cmd[2] << ", does not exist." << std::endl;
		return;
	}
	if (!getUserByNickname(cmd[1]))
	{
		sendNumericReply(usr, ERR_NOSUCHNICK, cmd[1]);
		return;
	}
	Channel* ch = getChannelByName(cmd[2]);
	if (ch->isInviteOnly() && !ch->isOperator(usr))
	{
		sendNumericReply(usr, ERR_CHANOPRIVSNEEDED, ch->getName());
		return;
	}
	if (!ch->isInChannel(usr->getNickname()))
	{
		sendNumericReply(usr, ERR_NOTONCHANNEL, ch->getName());
		return;
	}
	if (ch->isInChannel(cmd[1]))
	{
		std::string target = getUserByNickname(cmd[1])->getNickname();
		sendNumericReply(usr, ERR_USERONCHANNEL, target + " " + ch->getName());
		return;
	}
	// All ok, inviting
	std::cout << cmd[1] << " has been invited to chan " << ch->getName() << std::endl;
	if (ch->isInvited(getUserByNickname(cmd[1])))
		return;
	ch->invite(getUserByNickname(cmd[1]));
	std::string msg = "341 " + ch->getName();
	msg += " " + getUserByNickname(cmd[1])->getNickname();
	msg += " " + ch->getName();
	//sendChanReply(usr, msg, ch->getName());
	sendNumericReply(usr, RPL_INVITING, cmd[1] + " " + ch->getName());
	
}