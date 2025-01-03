#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h> // struct addrinfo
#include <sstream>
#include <cstring> // memset
#include "Client.hpp"

class Server {
public:
	Server();
	void RunServer( void );
	bool	SendMessage( int client_fd, std::string message ) const;
	// Getters
	int			GetPort( void ) const;
	std::string GetPassword( void ) const;
	~Server();

private:
	int					_port; // the server port
	int					_sockfd; 
	std::string			_password; // password to the server
	// std::vector<Client> _clients;
	std::map<std::string, Client> _clients; // map that takes a string [nickname] as a key and the value is the obj of the client with that nickname
};