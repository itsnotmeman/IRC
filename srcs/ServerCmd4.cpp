#include "Server.hpp"
	void Server::irc_mode(std::vector<std::string> cmd, int socket)
	{
		if (!getUserBySocket(socket))
			return;
		User* usr = getUserBySocket(socket);
		if (!usr->isRegistered())
			return;
		if (cmd.size() < 2)
		{
			sendNumericReply(usr, ERR_NEEDMOREPARAMS, "MODE");
			return;
		}
		if (cmd[1][0] != '#') // Les user modes ne sont pas supportés
			return;
		if (cmd.size() >= 2 && !getChannelByName(cmd[1]))
		{
			sendNumericReply(usr, ERR_NOSUCHCHANNEL, cmd[1]);
			return; 
		}
		if (cmd.size() == 2)
		{
			irc_mode2(cmd, socket);
			return;
		}
		std::vector<std::string> params;
		if (cmd.size() > 3)
			params = splitByChar(cmd[3], ',');
		char sign = '+';
		int i = 0;
    	while (i < (int)cmd[2].size())
		{
        	std::vector<std::string> newvec = cmd;
			std::string mod;
			std::string param = "";
			if (cmd[2][i] == '+' || cmd[2][i] == '-')
				sign = cmd[2][i];
        	else
			{
				mod.push_back(sign);
				newvec[2] = mod + cmd[2][i];
				if (params.size() == 0)
					param = "";
				else
				{
					param = params.front();
					params.erase(params.begin());
				}
				if (newvec.size() > 3)
					newvec[3] = param;
				
				irc_mode2(newvec, socket);
			}
			i++;
    	}
	}

    void Server::irc_mode2(std::vector<std::string> cmd, int socket)
	{
		if (!getUserBySocket(socket))
			return;
		User* usr = getUserBySocket(socket);
		if (!usr->isRegistered())
			return;
		if (cmd[1][0] != '#') // Les user modes ne sont pas supportés
			return;
		// MODE reçu avec 1 paramètre
		if (cmd.size() == 2)
		{
			Channel* ch = getChannelByName(cmd[1]);
			if (!ch)
			{
				sendNumericReply(usr, ERR_NOSUCHCHANNEL, cmd[1]);
				return; 
			}
			std::string str;
			if (ch->isInviteOnly())
				str += "i";
			if (ch->isTopicRestricted())
				str += "t";
			if (ch->isKeyProtected())
				str += "k";
			if (ch->hasUserLimit())
				str += "l";
			if (str.size())
				str = ch->getName() + " :+" + str;
			sendNumericReply(usr, RPL_CHANNELMODEIS, str);
		}
		// MODE reçu avec 2 paramètres ou +
		if (cmd.size() > 2)
		{
			Channel* ch = getChannelByName(cmd[1]);
			if (!ch)
			{
				sendNumericReply(usr, ERR_NOSUCHCHANNEL, cmd[1]);
				return; 
			}
			if (!ch->isOperator(usr))
			{
				sendNumericReply(usr, ERR_CHANOPRIVSNEEDED, cmd[1]);
				return; 
			}
			// Set invite only
			if (cmd[2] == "+i")
			{
				if (!ch->isInviteOnly())
				{
					ch->setInviteOnly(true);
				}			
			}
			else if (cmd[2] == "-i")
			{
				if (ch->isInviteOnly())
				{
					ch->setInviteOnly(false);
					ch->clearInvitees();
				}
			}
			// Set topic restricted
			else if (cmd[2] == "+t")
			{
				if ( !ch->isTopicRestricted())
				{
					ch->setTopicRestricted(true);
				}
			}
			else if (cmd[2] == "-t")
			{
				if (ch->isTopicRestricted())
					ch->setTopicRestricted(false);
			}
			// Set channel key
			else if (cmd[2] == "+k")
			{
				if (ch->isKeyProtected()){
					sendNumericReply(usr, ERR_KEYSET, cmd[1]);
					return;
				}
				else
				{
					ch->setKeyProtected(true);
					ch->setChannelKey(cmd[3]);
					std::cout << "key for chan " << ch->getName() << " set to" << cmd[3] << std::endl; 
				}
			}
			else if (cmd[2] == "-k")
			{
				ch->setKeyProtected(false);
			}
			// Promote/demote chanops
			else if (cmd[2] == "+o" || cmd[2] == "-o")
			{
				User* target = getUserByNickname(cmd[3]);
				if (!target)
				{
					sendNumericReply(usr, ERR_NOSUCHNICK, cmd[1]);
					return; 
				}
				if (!ch->isInChannel(target->getNickname()))
				{
					sendNumericReply(usr, ERR_USERNOTINCHANNEL, cmd[1] + " " + cmd[2]);
					return; 
				}
				// All ok
				if (cmd[2] == "+o" && !ch->isOperator(target))
				{
					ch->setOpStatus(target->getNickname(), true);
				}
				if (cmd[2] == "-o" && ch->isOperator(target))
				{
					ch->setOpStatus(target->getNickname(), false);
				}
			}
			// Set max users in chan. 0 means no limit
			else if (cmd[2] == "+l")
			{
				if (cmd.size() < 4)
				{
					sendNumericReply(usr, ERR_NEEDMOREPARAMS, "MODE");
					return;
				}
				int limit;
				std::istringstream iss(cmd[3]);
				if (iss >> limit)
				{
					if (limit < 0)
						limit = 0;
					ch->setUserLimit(limit);
				}
				else
				{
					sendNumericReply(usr, ERR_NEEDMOREPARAMS, "MODE");
					return;
				}
			}
			else if (cmd[2] == "-l")
			{
				ch->setUserLimit(0);
			}
			else
			{
				sendNumericReply(usr, ERR_UNKNOWNMODE, cmd[2]);
				return;
			}
			std::string msg = "MODE " + ch->getName();
			msg += " " + cmd[2];
			if (cmd.size() > 3)
				if (cmd[3] != "")
					msg += " :" + cmd[3];
			sendChanReply(usr, msg, ch->getName());
		}

	}
