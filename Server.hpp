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
#include "Channel.hpp"
#include "replies.hpp"

class Client;
class Channel;

class Server {
public:
	Server( std::string port, std::string password );
    void    RunServer( void );
	static void	SendMessage( int client_fd, std::string message ) ;
	void    broadcastNick(int fd, std::string nickReply);
	// Getters
	std::string	GetPort( void ) const;
	std::string GetPassword( void ) const;
	~Server();

	//added by soufiane
	std::vector<Channel>& getChannelList();
	Channel* getChannel(std::string channelName);
	Client*	getClientByNick(std::string nick);
	Client& getClient(int fd);
	std::vector<struct pollfd>& getClientsFds( void );
	std::string getHostName();
	void	setHostName(std::string name);
	void    bleachClient(int fd);
	void	removeChannle(std::string name);

private:
	std::string			hostName;
	std::string			_port; // the server port
	int					_sockfd;
	std::string			_password; // password to the server
	// std::vector<Client> _clients;
	static std::map<int, Client> _clients; // map that takes an int [fd] as a key and the value is the obj of the client with that fd
	static std::vector<struct pollfd> _fds;

	//added by soufiane
	std::vector<Channel> channels;
};

void    checkPassword(std::string& password);
