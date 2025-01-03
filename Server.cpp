#include "Server.hpp"

Server::Server() {
	this->_port = 6667;
	this->_sockfd = -1;
	this->_password = "dexter";
}

int	Server::GetPort( void ) const {
	return (this->_port);
}

std::string Server::GetPassword( void ) const {
	return (this->_password);
}

void Server::RunServer( void ) {
	// ⚠️ Maybe i should throw exceptions when error?

	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // fill my ip address automatically

	std::string str_port;
	std::stringstream ss;
	ss << this->_port;
	str_port = ss.str();
	ss.clear();

	getaddrinfo(NULL, str_port.c_str(), &hints, &res); // TODO error check it

	// create a socket for the server (it listens for incomming connections only!)
	this->_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (this->_sockfd == -1) {
		std::cerr << "Coudn't create a socket for the server" << std::endl;
		return ;
	}

	if (bind(this->_sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		std::cerr << "Coudn't bind the address" << std::endl;
		return ;
	}

	if (listen(this->_sockfd, SOMAXCONN) == -1) {
		std::cerr << "Coudn't listen" << std::endl;
		return ;
	}

	int client_fd;
	// i should change the params in accept (rn this is just a test)
	if ((client_fd = accept(this->_sockfd, 0, 0)) == -1) {
		std::cout << "Coudn't accept" <<std::endl;
		return ; // should be removed because the server shoudn't quit
	}

	std::string msg = "Welcome to the server";
	send(client_fd, msg.c_str(), msg.length(), 0); // should also check if it fails or not

	/*
	 * The idea rn is to get the information of the client the nickname then assign it to a client in the map
	 * since the map has a key of nickname we need to retrive the nickname and the information first;
	 * 
	 * This is used so we can have instante lookup times when we need to make direct messages
	*/

	std::cout << "sockfd => " << this->_sockfd << std::endl;
}