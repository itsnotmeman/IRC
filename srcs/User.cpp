#include "User.hpp"

User::~User(void){
	//std::cout << "user destructor called" << std::endl;
}
User::User(int socket, std::string host): socket(socket), hostname(host), 
		receivedMessageBuffer(""), sendMessageBuffer("") {	
	registered = false;
}
int User::getSocket(){return socket;}
std::string User::getHostname() const {return hostname;}
std::string User::getNickname() const {return nickname;}
std::string User::getUsername() const {return username;}
std::string User::getRealname() const {return realname;}
std::string User::getConnectionPassword() const {return connectionPassword;}
void User::setNickname(const std::string& nickname){ this->nickname = nickname;}
void User::setUsername(const std::string& username){this->username = username;}
void User::setRealname(const std::string& name){this->realname = name;}
void User::setConnectionPassword(const std::string& p){connectionPassword = p;}
std::string User::userURL() const {
	return this->getNickname() + "!" + this->getUsername() + "@" + this->getHostname();}
bool User::isRegistered() const {return registered;}
void User::setRegistered() {registered = true;}
bool User::checkRegistrationCriteria() const {
	if ((this->getNickname()).size() > 0 && (this->getUsername()).size() > 0
		&& (this->getRealname()).size() > 0)
		return true;
	return false;
}

// Debug only
void User::printUserId(){
	std::cout << this->getSocket() << ": " << this->getNickname();
	std::cout << " " << this->getUsername() << " " << this->getRealname();
	std::cout << " password " << this->getConnectionPassword();
	std::cout << std::endl;
}
