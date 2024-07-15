#include "Server.hpp"

void Server::executeBatch(std::string input, int socket)
{	
	std::vector<std::string> cmds = splitByChar(input, '\n');
	std::vector<std::string>::const_iterator it = cmds.begin();
	std::vector<std::string>::const_iterator ite = cmds.end();
	while (it != ite)
	{
		std::vector<std::string> parsedCommand = parseCmd(*it);
		std::cout << "Exec: '";
		printVector(parsedCommand);
		std::cout << "'" << std::endl;
		executeCommand(parsedCommand, socket);
		it++;
	}
}

std::vector<std::string> Server::parseCmd(std::string input)
{
	std::vector<std::string> parsedArgs;
	std::string lastArgument;
	std::string remainingArgs;

	// Remove possible trailing newlines
	while (!input.empty() &&
		(input[input.length() - 1] == '\n' ||
		 input[input.length() - 1] == '\r'))
    	input.erase(input.length() - 1);
	
	// Test if command has :prefix
	std::size_t delimiterPos = input.find(':');
	bool hasPrefix = false;
	if (delimiterPos == 0)
	{
		hasPrefix = true;
		input.erase(input.begin());
		delimiterPos = input.find(':');
  	}
	if (delimiterPos != std::string::npos) {
		lastArgument = input.substr(delimiterPos + 1);
		remainingArgs = input.substr(0, delimiterPos);
	}
	else
    	remainingArgs = input;
	parsedArgs = splitByChar(remainingArgs, ' ');
	if (lastArgument != "")
		parsedArgs.push_back(lastArgument);
    // ":" prefix is not used here, discard if exists
    if (hasPrefix == true)
		parsedArgs.erase(parsedArgs.begin());
  	return parsedArgs;
 }

std::vector<std::string> Server::splitByChar(std::string input, char s)
{
	std::vector<std::string> result;
	int startpos = 0;
	int offset = 0;
	int index = 0;
	std::string ss;

	ss += s;
	std::string::iterator it = input.begin();
	std::string::iterator ite = input.end();
	while (it != ite)
	{
		if (it == ite - 1 && *it == s)
		{
			offset = input.size() - startpos - 1;
			if (offset)
				result.push_back(input.substr(startpos, offset));
		}
		else if (it == ite - 1)
		{
			offset = input.size() - startpos;
			if (offset)
				result.push_back(input.substr(startpos, offset));
		}
		else if (*it == s)
		{
			index = std::distance(input.begin(), it);
			offset = index - startpos;
			if (offset)
				result.push_back(input.substr(startpos, offset));
			startpos = startpos + offset + 1;
		}
		it++;
	}
	if (result.size() == 0)
		result.push_back(input);
	return result;
}

void	Server::appendToUserBuffer(char const * receivedMessage, int const clientSocket)
{
	User *	user;

	user = getUserBySocket(clientSocket);
	if (user == NULL)
		throw (std::runtime_error("User does not exist anymore.1"));
	user->receivedMessageBuffer.append(receivedMessage);

	// // Add the ready to write client socket to the fd set.
	// FD_SET(clientSocket, &writeSockets);
}

void	Server::appendToOutputBuffer(char const * toSendMessage, int const clientSocket)
{
	User *	user;

	user = getUserBySocket(clientSocket);
	if (user == NULL)
		throw (std::runtime_error("User does not exist anymore.1"));
		
	user->sendMessageBuffer.append(toSendMessage);

	// Add the ready to write client socket to the fd set.
	FD_SET(clientSocket, &writeSockets);
}

void	Server::executeUserBuffer(int const clientSocket)
{
	size_t		pos;
	std::string	temp;
	User *		user;

	user = getUserBySocket(clientSocket);
	if (user == NULL)  // User is disconnected.
		return ;
	
	// Execute the command if the buffer contains a newline.
	pos = user->receivedMessageBuffer.find_last_of('\n');
	if (pos == std::string::npos)
		return ;

 	// // Remove client socket from the ready to write fd set.
	// FD_CLR(clientSocket, &writeSockets);

	// Store what remains after the last newline in a new buffer.
	temp = user->receivedMessageBuffer.substr(pos + 1);
	user->receivedMessageBuffer[pos + 1] = '\0';
	executeBatch(user->receivedMessageBuffer, clientSocket);
	user->receivedMessageBuffer = temp;
}
