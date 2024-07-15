#include "Server.hpp"

bool	Server::ctrlC = false;

/* Public member functions. */

void	Server::createSocket(void)
{
	// Create a new socket.
	_listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listeningSocket == -1)
		throw (std::runtime_error("Socket creation failed."));
	
	/* Set socket option so that the port is immediately available again
	 * after the program stops and is restarted. */
	const int	enable = 1;
	if (setsockopt(_listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
		throw (std::runtime_error("Set socket option failed."));

  	// Add new client socket to the socket list.
	openSockets.push_back(_listeningSocket);	

	// Make the socket non-blocking.
	if (fcntl(_listeningSocket, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("Failed to set the socket to non-blocking mode."));
}

void	Server::bindSocket(void)
{
	long				checkPort;
	struct sockaddr_in	address;

	// Check the port number.
	if (port.find_first_not_of("0123456789") != std::string::npos)
		throw (std::out_of_range("Invalid port number."));
	checkPort = strtol(port.c_str(), 0, 10);
	if (checkPort < MIN_PORT_VALUE || checkPort > MAX_PORT_VALUE)
		throw (std::out_of_range("Invalid port number. Choose one between 1024 and 65535."));

	// Bind the socket.
	address.sin_port = htons(checkPort);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	if (bind(_listeningSocket,
				reinterpret_cast<struct sockaddr *>(&address),
				sizeof(address)) == -1)
		throw (std::runtime_error("Binding the socket failed."));
}

Server::Server(std::string port, std::string password):
	port(port),
	password(password),
	creation_time(mc.getTimeStamp())
{
	// Initialize cmdMap<>
	initCmdMap();
	// Set server hostname
	this->hostname = "server_host";
    
	createSocket();
	bindSocket();
}

void	Server::listenSocket(void)
{
	if (listen(_listeningSocket, BACKLOG) == -1)
		throw (std::runtime_error("Listening on socket failed."));
}

void	Server::acceptClient(void)
{
	socklen_t			addrlen;
	struct sockaddr_in	clientAddress;

	addrlen = sizeof(clientAddress);
	_acceptedSocket = accept(_listeningSocket, reinterpret_cast<struct sockaddr *>(&clientAddress), &addrlen);
	if (_acceptedSocket == -1)
		throw (std::runtime_error("Accepting the connection failed."));

	// Make the socket non-blocking.
	if (fcntl(_acceptedSocket, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("Failed to set the socket to non-blocking mode."));
	
	openSockets.push_back(_acceptedSocket);  // Add new client socket to the socket list.
	FD_SET(_acceptedSocket, &currentSockets);  // Add the new client to the fd set.
	
	// Always update the largest fd number.
	if (_acceptedSocket > maxSocketNbr)
		maxSocketNbr = _acceptedSocket;

	addUser(_acceptedSocket, inet_ntoa(clientAddress.sin_addr));
}

void	Server::sigintHandler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM)
	{
		std::cout << "sigint or sigterm catched" << std::endl;  // TO DELETE.
		ctrlC = true;
	}
}

void	Server::receiveClientMessage(int clientSocket)
{
	memset(receivedMessage, 0, MESSAGE_SIZE);

	bytesReceived = recv(clientSocket, receivedMessage, MESSAGE_SIZE - 1, 0);
	if (bytesReceived == -1)
		throw (std::runtime_error("Receiving message failed."));
	else if (bytesReceived == 0)
	{
		deleteUser(clientSocket);
		return ;
	}

	receivedMessage[bytesReceived] = '\0';
	appendToUserBuffer(receivedMessage, clientSocket);
	executeUserBuffer(clientSocket);
}

void	Server::start(void)
{
	fd_set				readySockets;  // FOR SELECT().
	fd_set				readyWriteSockets;  // FOR SELECT().

	// Register new signal handling procedure for sigint (ctrl+c).
	if (signal(SIGINT, sigintHandler) == SIG_ERR)
		throw (std::runtime_error("Failed to register the new SIGINT handling procedure."));
	if (signal(SIGTERM, sigintHandler) == SIG_ERR)
		throw (std::runtime_error("Failed to register the new SIGTERM handling procedure."));

	maxSocketNbr = _listeningSocket;
	listenSocket();

	FD_ZERO(&currentSockets);  // Clear the fd set.
	FD_ZERO(&writeSockets);  // Clear the fd set.
	FD_SET(_listeningSocket, &currentSockets);  // Add _listeningSocket to the fd set.

	while (!ctrlC)
	{
		readySockets = currentSockets;  // Reinitialise readySockets.
		readyWriteSockets = writeSockets;  // Reinitialise readyWriteSockets.
		
		if (select(maxSocketNbr + 1, &readySockets, &readyWriteSockets, NULL, NULL) == -1)
		{
			if (ctrlC)
				break ;
			throw (std::runtime_error("Waiting for a file descriptor to become ready failed."));
		}

		// Check for ready file descriptors.
		for (int i = 0; i <= maxSocketNbr; i++)
		{
			// Check for ready file descriptors to read.
			if (FD_ISSET(i, &readySockets))
			{
				// Accept a new client connection.
				if (i == _listeningSocket)
					acceptClient();
				// Receive a message from a client.
				else
					receiveClientMessage(i);
			}
			// Check for ready file descriptors to write.
			if (FD_ISSET(i, &readyWriteSockets))
			{
				User * user = getUserBySocket(i);
				if (user == NULL)  // User is disconnected.
    				throw std::runtime_error("Sending message failed. User does not exist.");
				if (send(i, user->sendMessageBuffer.c_str(), user->sendMessageBuffer.length(), 0) == -1)
    				throw std::runtime_error("Sending message failed.");
				user->sendMessageBuffer.clear();
				FD_CLR(i, &writeSockets);
				// executeUserBuffer(i);
			}
		}
	}
}
