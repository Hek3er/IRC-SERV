#include "Client.hpp"
#include <cstddef>
#include <sched.h>

Client::Client( ) {
    this->_fd = -1;
    this->_registered = false;
}

Client::Client( int fd ) {
    this->_fd = fd;
    this->_registered = false;
}

Client::Client( int fd, std::string address ) {
    this->_fd = fd;
    this->_address = address;
    this->_registered = false;
}

Client::Client( int fd, std::string nickname, std::string username, std::string address ) {
	this->_fd = fd;
	this->_nickname = nickname;
	this->_username = username;
	this->_address = address;
	this->_registered = false;
}

std::string	Client::GetNickname( void ) const {
	return (this->_nickname);
}

std::string Client::GetUsername( void ) const {
	return (this->_username);
}

std::string Client::GetAddress( void ) const {
	return (this->_address);
}

int			Client::GetFd( void ) const {
	return (this->_fd);
}

void	Client::SetNickname( std::string nickname ) {
	this->_nickname = nickname;
}

void	Client::SetUsername( std::string username ) {
	this->_username = username;
}

void	Client::SetAddress( std::string address ) {
	this->_address = address;
}

void	Client::SetFd( int fd ) {
	this->_fd = fd;
}

void    Client::SwitchToRegistered( void ) {
    this->_registered = true;
}

bool    Client::IsRegistered( void ) const {
    return (this->_registered);
}

/* TODO:
	* Nicknames are non-empty strings with the following restrictions:
	* They MUST NOT contain any of the following characters: space (' ', 0x20), comma (',', 0x2C), asterisk ('*', 0x2A), question mark ('?', 0x3F), exclamation mark ('!', 0x21), at sign ('@', 0x40). ✅
	* They MUST NOT start with any of the following characters: dollar ('$', 0x24), colon (':', 0x3A). ✅
	* They MUST NOT start with a character listed as a channel type, channel membership prefix, or prefix listed in the IRCv3 multi-prefix Extension. ❌ [not all are implemented]
	* They SHOULD NOT contain any dot character ('.', 0x2E). ✅
*/

bool Client::CheckNickname( std::string nickname ) const {

	if (nickname == "" || nickname[0] == '$' || nickname[0] == ':' || nickname[0] == '&' || nickname[0] == '#') {
		return false;
	}

	if (nickname.find(' ') != std::string::npos || 
		nickname.find(',') != std::string::npos ||
		nickname.find('*') != std::string::npos || 
		nickname.find('?') != std::string::npos ||
		nickname.find('!') != std::string::npos ||
		nickname.find('@') != std::string::npos ||
		nickname.find('.') != std::string::npos) {
			return false;
		}
	return true;
}