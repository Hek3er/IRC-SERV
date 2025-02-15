	#pragma once
	#include <iostream>
	#include <arpa/inet.h>
	#include <queue>
	#include "Server.hpp"
	#include <exception>

enum LEVEL {
	ZERO,
	ONE,
	TWO
};

class Server;

class Client {
	public:
		Client( );
		Client( int fd );
		Client( int fd, std::string address );
		Client( int fd, std::string nickname, std::string username, std::string address );

		// Getters
		std::string	GetNickname( void ) const;
		std::string GetUsername( void ) const;
		std::string GetAddress( void ) const;
		int			GetFd( void ) const;
		LEVEL		getAuthLevel( void ) const;

		void   SwitchToRegistered( void );
		bool   IsRegistered( void ) const;

		//Setters
		void	SetNickname( std::string nickname );
		void	SetUsername( std::string username );
		void	SetAddress( std::string address );
		void	SetFd( int fd );
		void	SetAuthLevel( int level );

		void	SendMessage( const std::string& msg ) const;
		void	QueueMessage(const std::string& msg);
		std::string GetNextMessage( void );
		bool	HasMessages( void ) const;

		void    StoreBuffer( char *buff, int size );
		bool    IsBufferReady( void ) const;
		std::string   GetBuffer( void );

	private:
		LEVEL		_level;
		bool		_registered;
		int			_fd;
		std::string _nickname;
		std::string _username;
		std::string	_address;
		std::queue<std::string> _messages;
		std::string _buffer;
		bool        _messageCompleted;
};

bool	passCmd(Server& irc_srv, Client& clt, std::vector<std::string>& args);
bool	nickCmd(Server& irc_srv, Client& clt, std::map<int, Client> &clients, std::vector<std::string>& args);
