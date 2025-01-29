	#pragma once
	#include <iostream>
	#include <arpa/inet.h>
	#include <queue>
	#include "Server.hpp"

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
		
		void   SwitchToRegistered( void );
		bool   IsRegistered( void ) const;

		//Setters
		void	SetNickname( std::string nickname );
		void	SetUsername( std::string username );
		void	SetAddress( std::string address );
		void	SetFd( int fd );

		bool	CheckNickname( std::string nickname ) const;
		void	SendMessage( const std::string& msg ) const;
		void	QueueMessage(const std::string& msg);
		std::string GetNextMessage( void );
		bool	HasMessages( void ) const;

	private:
		bool        _registered;
		int			_fd;
		std::string _nickname;
		std::string _username;
		std::string	_address;
		std::queue<std::string> _messages;
	};