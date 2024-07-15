#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>

#include "User.hpp"
#include "Utility.hpp"

class Channel {
private:
    std::string name;
    std::string topic;
    bool key_protected;
    std::string channel_key;
    bool invite_only;
    bool topic_restricted;
    long unsigned int max_users;
    std::vector<std::string> users;
    std::vector<std::string> invitees;

public:
    Channel(std::string name);
	
    std::string getName();
    std::string getTopic();
    bool getTopicRestricted();
    void setTopic(const std::vector<std::string> cmds);
    bool isInChannel(std::string nick);
    void addUser(std::string);
    std::vector<std::string>& getUsers();
    std::string getUserList();
    bool isInviteOnly();
    bool isInvited(User*);
    std::string getKey();
    void removeUser(User*);
    bool isOperator(User*);
    void invite(User*);
    bool isTopicRestricted();
    bool hasUserLimit();
    void setInviteOnly(bool);
    void setTopicRestricted(bool);
    void setChannelKey(std::string);
    bool isKeyProtected();
    void setKeyProtected(bool);
    long unsigned int getMaxUsers();
    void clearInvitees();
    void setOpStatus(std::string, bool);
    void setUserLimit(int maxUsers);
};

#endif
