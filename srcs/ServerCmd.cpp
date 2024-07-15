#include "Server.hpp"

void Server::initCmdMap() {
	
	cmdMap["PASS"] = &Server::irc_pass;
	cmdMap["USER"] = &Server::irc_user;
	cmdMap["NICK"] = &Server::irc_nick;
	cmdMap["PRIVMSG"] = &Server::irc_privmsg;
	cmdMap["NOTICE"] = &Server::irc_notice;
	cmdMap["JOIN"] = &Server::irc_join;
	cmdMap["PING"] = &Server::irc_ping;
	cmdMap["LIST"] = &Server::irc_list;
	cmdMap["NAMES"] = &Server::irc_names;
	cmdMap["KICK"] = &Server::irc_kick;
	cmdMap["INVITE"] = &Server::irc_invite;
	cmdMap["TOPIC"] = &Server::irc_topic;
	cmdMap["MODE"] = &Server::irc_mode;
}

void Server::executeCommand(std::vector<std::string> cmd, int socket)
{
	// Convert cmd[0] (command name) to uppercase
	std::string::iterator it = cmd[0].begin();
	std::string::iterator ite = cmd[0].end();
	while (it != ite)
	{
		*it = std::toupper(*it);
		it++;
	}
	// Search for command name in cmdMap<>
	if (cmdMap.size() == 0)
		std::cout << "cmdMap is empty, did you forget to initCmdMap() in Server constructor?" << std::endl;
	std::map<std::string, cmdFct>::iterator mapIt = cmdMap.find(cmd[0]);
	
	// Execute command
	if (mapIt != cmdMap.end()) {
        cmdFct cmdFunc = mapIt->second;
        (this->*cmdFunc)(cmd, socket);
    } else {
        std::cout << "Command not found!" << std::endl;
    }

}

// PING
void Server::irc_ping(std::vector<std::string> cmd, int socket)
{
	if (cmd.size() > 1 && cmd[1] != "")
		sendMessage(socket, "PONG " + cmd[1]);
	else
		sendMessage(socket, "PONG");
	return;
}	

// NAMES
void Server::irc_names(std::vector<std::string> cmd, int socket)
{
	User* usr = getUserBySocket(socket);
	if (!usr || !usr->isRegistered())
			return;
	if (cmd.size() > 1)
	{
		std::vector<std::string> chans = splitByChar(cmd[1], ',');
		std::vector<std::string>::iterator it = chans.begin();
		std::vector<std::string>::iterator ite = chans.end();
		while (it != ite)
		{
			if (getChannelByName(*it))
			{
				std::string userlist = getChannelByName(*it)->getUserList();
				sendNumericReply(usr, RPL_NAMREPLY, userlist);
			}
			it++;
		}
		sendNumericReply(usr, RPL_ENDOFNAMES, cmd[1]);
	}
	else if (cmd.size() == 1)
	{
		std::vector<Channel*>::iterator it = channels.begin();
		std::vector<Channel*>::iterator ite = channels.end();
		while (it != ite)
		{
			std::string userlist = (*it)->getUserList();
			sendNumericReply(usr, RPL_NAMREPLY, userlist);
			it++;
		}
		sendNumericReply(usr, RPL_ENDOFNAMES, "*");
	}
}

// LIST
void Server::irc_list(std::vector<std::string> cmd, int socket)
{
	User* usr = getUserBySocket(socket);
	if (!usr || !usr->isRegistered())
			return;
	if (cmd.size() > 1)
	{
		std::vector<std::string> chans = splitByChar(cmd[1], ',');
		std::vector<std::string>::iterator it = chans.begin();
		std::vector<std::string>::iterator ite = chans.end();
		while (it != ite)
		{
			if (getChannelByName(*it))
			{
				Channel *	channel = getChannelByName(*it);
				sendNumericReply(usr, RPL_LIST, (*it) + " :" + channel->getTopic());
			}
			it++;
		}
		sendNumericReply(usr, RPL_LISTEND, "");
	}
	else if (cmd.size() == 1)
	{
		std::vector<Channel*>::iterator it = channels.begin();
		std::vector<Channel*>::iterator ite = channels.end();
		while (it != ite)
		{
			sendNumericReply(usr, RPL_LIST, (*it)->getName() + " :" + (*it)->getTopic());
			it++;
		}
		sendNumericReply(usr, RPL_LISTEND, "");
	}
}

// TOPIC
void Server::irc_topic(std::vector<std::string> cmd, int socket)
{	
	User*		usr;
	Channel *	channel;

	usr = getUserBySocket(socket);
	if (!usr || !usr->isRegistered())
		return;
	//if (!usr)
	//	throw (std::runtime_error("User does not exist anymore."));
	if (cmd.size() < 2)
	{
		sendNumericReply(usr, ERR_NEEDMOREPARAMS, "TOPIC");
		return ;
	}
	channel = getChannelByName(cmd[1]);
	if (!channel)
		sendNumericReply(usr, ERR_NOSUCHCHANNEL, cmd[1]);
	else if (!channel->isInChannel(usr->getNickname()))
		sendNumericReply(usr, ERR_NOTONCHANNEL, cmd[1]);
	else if (cmd.size() == 2)  // Show topic information.
	{
		if (channel->getTopic() == "")
			sendNumericReply(usr, RPL_NOTOPIC, ":" + channel->getName());
		else
			sendNumericReply(usr, RPL_TOPIC, ":" + channel->getTopic());
	}
	else  // Modify topic information.
	{
		if (channel->getTopicRestricted() && !channel->isOperator(usr))
			sendNumericReply(usr, ERR_CHANOPRIVSNEEDED, cmd[1]);
		else
		{
			channel->setTopic(cmd);
			sendChanReply(usr, "TOPIC " + channel->getName() + " :" + channel->getTopic(), channel->getName());
		}
	}
}
