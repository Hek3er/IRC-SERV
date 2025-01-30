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
#include <poll.h>

class Client;

class Server {
public:
	Server();
    void    RunServer( void );
	static void	SendMessage( int client_fd, std::string message ) ;
	// Getters
	int			GetPort( void ) const;
	std::string GetPassword( void ) const;
	~Server();

private:
	int					_port; // the server port
	int					_sockfd; 
	std::string			_password; // password to the server
	// std::vector<Client> _clients;
	static std::map<int, Client> _clients; // map that takes an int [fd] as a key and the value is the obj of the client with that fd
	static std::vector<struct pollfd> _fds;
};