#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

class User {
private:
    int socket;
    bool registered;
    std::string hostname;   // usr_hostname
    std::string nickname;   // "joe"
    std::string username;   // "jdoe"
    std::string realname;   // "John Doe"
    std::string connectionPassword;
	
    User();
	
public:
    std::string receivedMessageBuffer;
    std::string sendMessageBuffer;

	~User();
    User(int socket, std::string host);
    
    int getSocket();
    std::string getNickname() const;
    std::string getUsername() const;
    std::string getRealname() const;
    std::string getHostname() const;
    std::string getConnectionPassword() const;

    void setNickname(const std::string&);
    void setUsername(const std::string&);
    void setRealname(const std::string&);
    void setConnectionPassword(const std::string&);
 
    std::string userURL() const; // returns "nick!username@usr_host"
    bool isRegistered() const;
    void setRegistered();

    bool checkRegistrationCriteria() const;

    // Debug only
    void printUserId();
};

#endif