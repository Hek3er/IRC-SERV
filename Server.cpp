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

void Server::InitServer( void ) {

}