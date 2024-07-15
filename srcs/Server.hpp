#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <list>
#include <map>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <exception>
#include <cstdlib>
#include <sys/socket.h>  // for socket(), bind()
#include <sys/types.h>  // for bind()
#include <sys/stat.h>  // for fstat()
#include <netdb.h>  // for getprotobyname()
#include <arpa/inet.h>  // for htons()
#include <unistd.h>  // for close()
#include <fcntl.h>  // for fcntl()
#include <cstring>  // for strcmp()
#include <fstream>
#include <algorithm>  // for for_each()
#include <csignal>  // for signal()
#include <cerrno>  // for errno

#include "User.hpp"
#include "Channel.hpp"
#include "MessageCenter.hpp"
#include "Utility.hpp"

#define BACKLOG 5
#define MIN_PORT_VALUE 1024
#define MAX_PORT_VALUE 65535
#define	MESSAGE_SIZE 512

class Server {

private:
    static bool ctrlC;
    int		    _listeningSocket;
	int		    _acceptedSocket;
	int		    maxSocketNbr;
    fd_set      currentSockets;
    fd_set      writeSockets;
	char		receivedMessage[MESSAGE_SIZE];
	ssize_t		bytesReceived;
    std::list<int> openSockets;
	std::string port;
    std::string password;
    std::string hostname;
    std::string creation_time;
    std::vector<User*> users;
    std::vector<Channel*> channels;
    MessageCenter mc;
    
    typedef void (Server::*cmdFct)(std::vector<std::string> cmd, int socket);
	std::map<std::string, cmdFct> cmdMap;

    Server();
    Server(Server const & src);
    Server& operator=(Server const & rhs);

public:
    // Server.cpp
    ~Server();
    void addUser(int socket, std::string host);
    User* getUserBySocket(int socket) const;
    User* getUserByNickname(std::string nickname) const;
    int getSocketByNickname(std::string nickname) const;
    int getUserCount();
    int getChannelCount();
    void deleteEmptyChannels();
    void deleteUser(int socket);

    void sendChanReply(User* usr, std::string command, std::string chan_name);
	void sendNumericReply(User* dst, std::string code, std::string param);
	void sendPrivmsg(User* usr, std::string dest, std::string text);
    void sendNotice(User*, std::string dest, std::string text);
    void sendMsgToSingle(User* usr, std::string dest, std::string cmd, std::string text);
    void sendMessage(int socket, std::string message);
    Channel* getChannelByName(std::string);
    bool isInChannel(User*, Channel*);

    // ServerNetworking.cpp
    Server(std::string portNbr, std::string password);
    void    start(void);
    void    createSocket(void);
    void    bindSocket(void);
    void    listenSocket(void);
    void    acceptClient(void);
    void	receiveClientMessage(int clientSocket);
    static void	sigintHandler(int signum);

    // ServerParse.cpp
    void executeBatch(std::string input, int socket);
	std::vector<std::string> parseCmd(std::string cmd);
    std::vector<std::string> splitByChar(std::string input, char s);
    void	appendToUserBuffer(char const * receivedMessage, int const clientSocket);
    void	executeUserBuffer(int const clientSocket);
    void	appendToOutputBuffer(char const * toSendMessage, int const clientSocket);

    // Debug only
    void printVector(std::vector<std::string>);

    // ServerCmd.cpp
    void initCmdMap();
    void executeCommand(std::vector<std::string> cmd, int socket);
    void irc_ping(std::vector<std::string> cmd, int socket);
    void irc_names(std::vector<std::string> cmd, int socket);
    void irc_list(std::vector<std::string> cmd, int socket);
    void irc_topic(std::vector<std::string> cmd, int socket);

    
    // ServerCmd1.cpp
    void irc_pass(std::vector<std::string> cmd, int socket);
    void irc_nick(std::vector<std::string> cmd, int socket);
    void irc_user(std::vector<std::string> cmd, int socket);
    void checkRegistration(User* usr);

    // ServerCmd2.cpp
    void irc_privmsg(std::vector<std::string> cmd, int socket);
    void irc_notice(std::vector<std::string> cmd, int socket);
    void irc_join(std::vector<std::string> cmd, int socket);

    //ServerCmd3.cpp
    void irc_kick(std::vector<std::string> cmd, int socket);
    void irc_invite(std::vector<std::string> cmd, int socket);

    //ServerCmd4.cpp
    void irc_mode(std::vector<std::string> cmd, int socket);
    void irc_mode2(std::vector<std::string> cmd, int socket);
    static void  freeUsers(User* user);
};

#endif
