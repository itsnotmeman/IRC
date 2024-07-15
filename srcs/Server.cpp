#include "Server.hpp"

/* Private member functions. */

Server::Server(void) {}

Server::Server(Server const &src) { *this = src; }

Server &Server::operator=(Server const &rhs)
{
  if (this != &rhs)
  {
    ;
  }
  return (*this);
}

/* Public member functions. */

void Server::freeUsers(User *user)
{
  delete user;
}

Server::~Server()
{
  // Delete all Users and Channels
  for_each(openSockets.begin(), openSockets.end(), close);
  for_each(users.begin(), users.end(), freeUsers); // PUT THIS IN USER CLASS?

  std::vector<Channel *>::iterator it = channels.begin();
  std::vector<Channel *>::iterator ite = channels.end();
  while (it != ite)
  {
    delete (*it);
    it++;
  }
}

void Server::addUser(int socket, std::string host)
{
  users.push_back(new User(socket, host));
}

Channel *Server::getChannelByName(std::string name)
{
  std::vector<Channel *>::const_iterator it = channels.begin();
  std::vector<Channel *>::const_iterator ite = channels.end();
  while (it != ite)
  {
    if (Utility::noCase((*it)->getName(), name))
    {
      return *it;
    }
    it++;
  }
  return NULL;
}

User *Server::getUserBySocket(int socket) const
{
  std::vector<User *>::const_iterator it = users.begin();
  std::vector<User *>::const_iterator ite = users.end();
  while (it != ite)
  {
    if ((*it)->getSocket() == socket)
      return *it;
    it++;
  }
  return NULL;
}

User *Server::getUserByNickname(std::string nick) const
{
  std::vector<User *>::const_iterator it = users.begin();
  std::vector<User *>::const_iterator ite = users.end();
  if (nick[0] == '@')
    nick.erase(0, 1);
  while (it != ite)
  {
    if (Utility::noCase((*it)->getNickname(), nick))
      return *it;
    it++;
  }
  return NULL;
}

int Server::getSocketByNickname(std::string nick) const
{
  std::vector<User *>::const_iterator it = users.begin();
  std::vector<User *>::const_iterator ite = users.end();
  if (nick[0] == '@')
    nick.erase(0, 1);
  while (it != ite)
  {
    if (Utility::noCase((*it)->getNickname(), nick))
      return (*it)->getSocket();
    it++;
  }
  return 0;
}

int Server::getUserCount()
{
  return users.size();
}

int Server::getChannelCount()
{
  return channels.size();
}

void Server::deleteEmptyChannels()
{
  std::vector<Channel *>::iterator it = channels.begin();
  std::vector<Channel *>::iterator ite = channels.end();
  while (it != ite)
  {
    Channel *ch = *it;
    if (ch->getUsers().size() == 0)
    {
      delete ch;
      channels.erase(it);
    }
    it++;
  }
}

void Server::printVector(std::vector<std::string> v)
{
  std::vector<std::string>::iterator it = v.begin();
  std::vector<std::string>::iterator ite = v.end();
  int i = 0;
  while (it != ite)
  {
    std::cout << i++ << " ";
    std::cout << *it << " ";
    it++;
  }
  std::cout << std::endl;
}

void Server::sendChanReply(User *usr, std::string command, std::string chan_name)
{
  std::string outmsg = ":" + usr->userURL() + " ";
  outmsg += command;
  std::vector<std::string> recipients = getChannelByName(chan_name)->getUsers();
  std::vector<std::string>::iterator it = recipients.begin();
  std::vector<std::string>::iterator ite = recipients.end();
  while (it != ite)
  {
    sendMessage(getUserByNickname(*it)->getSocket(), outmsg);
    it++;
  }
}

void Server::sendNumericReply(User *dst, std::string code, std::string p)
{
  std::string msg = mc.getNumReply(code);
  size_t startPos = msg.find("<P>");
  if (startPos != std::string::npos)
    msg.replace(startPos, 3, p);

  std::string nick = dst->getNickname();
  if (nick == "")
    nick = "*";

  std::string outmsg = ":" + hostname + " " + code + " " + nick + " " + msg;
  sendMessage(dst->getSocket(), outmsg);
}

void Server::sendPrivmsg(User *usr, std::string dest, std::string text)
{
  std::vector<std::string> recipients = splitByChar(dest, ',');
  std::vector<std::string>::iterator it = recipients.begin();
  std::vector<std::string>::iterator ite = recipients.end();
  while (it != ite)
  {
    sendMsgToSingle(usr, *it, "PRIVMSG", text);
    it++;
  }
}

void Server::sendNotice(User *usr, std::string dest, std::string text)
{
  std::vector<std::string> recipients = splitByChar(dest, ',');
  std::vector<std::string>::iterator it = recipients.begin();
  std::vector<std::string>::iterator ite = recipients.end();
  while (it != ite)
  {
    sendMsgToSingle(usr, *it, "NOTICE", text);
    it++;
  }
}

void Server::sendMsgToSingle(User *usr, std::string dest, std::string cmd, std::string text)
{
  std::string prefix = ":" + usr->userURL();
  std::vector<std::string> recipients;

  bool toChan = false;
  if (dest[0] != '#')
    recipients.push_back(dest);
  else
  {
    toChan = true;
    if (!getChannelByName(dest))
    {
      sendNumericReply(usr, ERR_NOSUCHNICK, dest);
      return;
    }
    recipients = getChannelByName(dest)->getUsers();
  }
  std::string msg;
  std::vector<std::string>::iterator it = recipients.begin();
  std::vector<std::string>::iterator ite = recipients.end();
  while (it != ite)
  {
    std::string recipient = *it;
    if (!recipient.empty() && recipient[0] == '@')
      recipient = recipient.substr(1);
    msg = prefix + " " + cmd + " " + dest;
    if (!text.empty())
      msg += " :" + text;
    if (getSocketByNickname(recipient))
    {
      if (getUserByNickname(recipient) != usr || !toChan)
        sendMessage(getSocketByNickname(recipient), msg);
    }
    else if (cmd == "PRIVMSG")
      sendNumericReply(usr, ERR_NOSUCHNICK, recipient);
    it++;
  }
}

void Server::sendMessage(int socket, std::string message)
{
  std::cout << "OUT[" << Utility::itoa(socket) << "] " << message << std::endl;
  message = message + '\r' + '\n';

	appendToOutputBuffer(message.c_str(), socket);
  // if (send(socket, message.c_str(), message.length(), 0) == -1)
    // throw std::runtime_error("Sending message failed.");
}

void Server::deleteUser(int socket)
{
  User *user = getUserBySocket(socket);
  if (!user)
    throw(std::runtime_error("User does not exist anymore."));

  // Remove user from users vector.
  std::vector<User *>::iterator position;
  position = find(users.begin(), users.end(), user);
  users.erase(position); // Remove the client socket from the users list.

  // Remove user from socket list.
  std::list<int>::iterator position2;
  position2 = find(openSockets.begin(), openSockets.end(), socket);
  openSockets.erase(position2);    // Remove the client socket from the socket list.
  FD_CLR(socket, &currentSockets); // Remove the client from the fd set.
  FD_CLR(socket, &writeSockets);   // Remove the client from the fd set.
  if (close(socket) == -1)
    throw(std::runtime_error("Closing client socket failed."));

  // Remove user from channels vector.
  std::vector<Channel *>::iterator it = channels.begin();
  std::vector<Channel *>::iterator ite = channels.end();
  std::string nickname = user->getNickname();
  while (it != ite)
  {
    if ((*it)->isInChannel(nickname))
      (*it)->removeUser(user);
    sendChanReply(user, "QUIT: disconnected ", (*it)->getName());
    it++;
  }
  deleteEmptyChannels();
  delete user;
}