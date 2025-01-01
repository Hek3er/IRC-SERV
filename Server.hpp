#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <unistd.h>
#include "Client.hpp"

class Server {
public:
	Server();
	void InitServer( void );
	// Getters
	int			GetPort( void ) const;
	std::string GetPassword( void ) const;

private:
	int					_port; // the server port
	int					_sockfd; 
	std::string			_password; // password to the server
	// std::vector<Client> _clients;
	std::unordered_map<std::string, Client> _clients; // map that takes a string [nickname] as a key and the value is the obj of the client with that nickname
};