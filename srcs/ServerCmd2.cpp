#include "Server.hpp"

// PRIVMSG
void Server::irc_privmsg(std::vector<std::string> cmd, int socket)
{
	User* usr = getUserBySocket(socket);
	if (!usr->isRegistered())
	{
		sendNumericReply(usr, ERR_NOTREGISTERED, "PRIVMSG");
		return; 
	}
	if (cmd.size() == 1)
	{
		sendNumericReply(usr, ERR_NORECIPIENT, "PRIVMSG");
		return; 
	}
	sendPrivmsg(usr, cmd[1], cmd[2]);
	return;
}

void Server::irc_notice(std::vector<std::string> cmd, int socket)
{
	User* usr = getUserBySocket(socket);
	if (cmd.size() < 3)
	{
		//sendNumericReply(usr, ERR_NORECIPIENT, "NOTICE");
		return; 
	}
	if (!usr->isRegistered())
	{
		//sendNumericReply(usr, ERR_NOTREGISTERED, "NOTICE");
		return; 
	}
	sendNotice(usr, cmd[1], cmd[2]);
	return;
}

// JOIN
void Server::irc_join(std::vector<std::string> cmd, int socket)
{
    User* usr = getUserBySocket(socket);
    if (!usr || !usr->isRegistered())
        return;
    if (cmd.size() < 2)
    {
        sendNumericReply(usr, ERR_NEEDMOREPARAMS, "JOIN");
        return;
    }
    std::vector<std::string> subpasses;
    if (cmd.size() > 2)
    {
        subpasses = splitByChar(cmd[2], ',');
    }
    std::vector<std::string> recipients = splitByChar(cmd[1], ',');
    std::vector<std::string>::iterator it = recipients.begin();
    std::vector<std::string>::iterator ite = recipients.end();
    
    while (it != ite)
    {
        std::string nextpass;
        if (!getChannelByName(*it))
        {
            if (!Utility::isValidChannelName(*it))
            {
                sendNumericReply(usr, ERR_NOSUCHCHANNEL, *it);
                it++;
                continue;
            }
            Channel* newchan = new Channel(*it);
            newchan->addUser('@' + usr->getNickname());
            channels.push_back(newchan);
            sendChanReply(usr, "JOIN " + newchan->getName(), *it);
            executeCommand(parseCmd("NAMES " + *it), usr->getSocket());
            std::cout << usr->getNickname() << " has created chan " << newchan->getName() << ". Users: ";
            printVector(newchan->getUsers());
            std::cout << std::endl;
        }
        else
        {
			Channel* ch = getChannelByName(*it);
            // If user is already in channel
            if (ch->isInChannel(usr->getNickname()))
                return;
            // If channel is invite-only
			if (ch->isInviteOnly() && !ch->isInvited(usr) && !ch->isKeyProtected())
            {
                sendNumericReply(usr, ERR_INVITEONLYCHAN, *it);
                it++;
                continue;
            }
			// If channel is password protected
            if (!ch->isInvited(usr) && ch->isKeyProtected())
            {
                if (subpasses.size() == 0)
                    nextpass ="";
                else
                {
                    nextpass = subpasses.front();
                    subpasses.erase(subpasses.begin());
                }
                std::cout << "Channel has key " + ch->getKey() + " submitted: " << nextpass << std::endl;
				if (ch->getKey() != nextpass)
                {
                    sendNumericReply(usr, ERR_BADCHANNELKEY, *it);
                    it++;
                    continue;
                }
            }
			// Err if channel is full
			if (ch->getMaxUsers() && (ch->getUsers()).size() >= ch->getMaxUsers())
			{
				sendNumericReply(usr, ERR_CHANNELISFULL, *it);
                it++;
                continue;
			}
			// All ok join if user isn't already on channel
            if (!ch->isInChannel(usr->getNickname()))
            {
                ch->addUser(usr->getNickname());
                sendChanReply(usr, "JOIN " + ch->getName(), *it);
                executeCommand(parseCmd("TOPIC " + *it), usr->getSocket());
                executeCommand(parseCmd("NAMES " + *it), usr->getSocket());
                std::cout << usr->getNickname() << " has joined " << *it << std::endl;
            }
        }
        it++;
    }   
}
