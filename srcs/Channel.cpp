#include "Channel.hpp"

#include <string>

Channel::Channel(std::string name): topic("") {
	this->name = name;
	invite_only = false;
	topic_restricted = true;
	max_users = 25;
	key_protected = false;
	channel_key = "";

}

std::string Channel::getName(){return name;}
std::string Channel::getTopic(){return topic;}
bool Channel::getTopicRestricted(){return topic_restricted;}
void Channel::setTopic(std::vector<std::string> cmds)
{
	std::vector<std::string>::iterator it = cmds.begin() + 2;  // Start at the first topic argument.
	std::vector<std::string>::iterator ite = cmds.end();
	this->topic.clear();
	while (it != ite)
	{
		this->topic.append(*it);
		++it;
	}
}
void Channel::addUser(std::string nick) {users.push_back(nick);}
std::vector<std::string>& Channel::getUsers() {return users;}
bool Channel::isInChannel(std::string nick) {
	std::vector<std::string>::const_iterator it = users.begin();
	std::vector<std::string>::const_iterator ite = users.end();
	while (it != ite)
	{
		std::string user = *it;
		if (user[0] == '@')
			user.erase(0,1);
		if (Utility::noCase(user, nick))
			return true;
		it++;
	}
	return false;
}

std::string Channel::getUserList() {
	std::string str = "= " + name + " :";
	std::vector<std::string>::iterator it = users.begin();
	std::vector<std::string>::iterator ite = users.end();
	while (it != ite)
	{
		str += *it + " ";
		it++;
	}
	return str;
}

bool Channel::isInviteOnly() {
	if (invite_only)
		return true;
	return false;
}

bool Channel::isInvited(User* usr) {
	std::vector<std::string>::iterator it = invitees.begin();
	std::vector<std::string>::iterator ite = invitees.end();
	while (it != ite)
	{
		if (*it == usr->getNickname())
			return true;
		it++;
	}
	return false;
}

std::string Channel::getKey(){return channel_key;}

void Channel::removeUser(User* usr)
{
	std::vector<std::string>::iterator it = users.begin();
	std::vector<std::string>::iterator ite = users.end();
	while (it != ite)
	{
		std::string user = *it;
		if (user[0] == '@')
			user.erase(0,1);

		if (Utility::noCase(user, usr->getNickname()))
		{
			users.erase(it);
			return;
		}
		it++;
	}
}

bool Channel::isOperator(User* usr)
{
	std::vector<std::string>::iterator it = users.begin();
	std::vector<std::string>::iterator ite = users.end();
	bool isOp;
	while (it != ite)
	{
		isOp = false;
		std::string user = *it;
		if (user[0] == '@')
		{
			isOp = true;
			user.erase(0,1);
		}
		if (isOp && Utility::noCase(user, usr->getNickname()))
			return true;
		it++;
	}
	return false;
}

void Channel::invite(User* usr) {
	if (!isInvited(usr))
		invitees.push_back(usr->getNickname());
}

bool Channel::isTopicRestricted(){
	if (this->topic_restricted)
		return true;
	return false;
}

bool Channel::hasUserLimit(){
	if (this->max_users == 0)
		return false;
	return true;
}

void Channel::setInviteOnly(bool v)
{
	if (v == true)
		invite_only = true;
	else
		invite_only = false;

}

void Channel::setTopicRestricted(bool v)
{
	if (v == true)
		topic_restricted = true;
	else
		topic_restricted = false;

}

void Channel::setChannelKey(std::string key)
{
	channel_key = key;
}

bool Channel::isKeyProtected(){
	if (key_protected == true)
		return true;
	return false;
}

void Channel::setKeyProtected(bool v)
{
	if (v == true)
	{
		key_protected = true;
	}
	else
		key_protected = false;
}

long unsigned int Channel::getMaxUsers(){
	return max_users;
}

void Channel::clearInvitees() {
	invitees.clear();
}

void Channel::setOpStatus(std::string nick, bool v)
{
	std::vector<std::string>::iterator it = users.begin();
	std::vector<std::string>::iterator ite = users.end();
	while (it != ite)
	{
		bool isOp = false;
		std::string curr = *it;
		if (curr[0] == '@')
		{
			isOp = true;
			curr.erase(0,1);
		}
		if (curr == nick)
		{
			if (v == true && !isOp) // promote
				*it = "@" + *it;
			if (v == false && isOp) // demote
				*it = curr;
		}
		it++;
	}
}

void Channel::setUserLimit(int maxUser)
{
	max_users = maxUser;
}
