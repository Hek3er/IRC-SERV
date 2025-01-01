#pragma once
#include <iostream>
#include <arpa/inet.h>

class Client {
public:
	Client( int fd, std::string nickname, std::string username, std::string address );

	// Getters
	std::string	GetNickname( void ) const;
	std::string GetUsername( void ) const;
	std::string GetAddress( void ) const;
	int			GetFd( void ) const;

	//Setters
	void	SetNickname( std::string nickname );
	void	SetUsername( std::string username );
	void	SetAddress( std::string address );
	void	SetFd( int fd );

private:
	int			_fd;
	std::string _nickname;
	std::string _username;
	std::string	_address;
};