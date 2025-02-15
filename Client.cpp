#include "Client.hpp"
#include "Server.hpp"
#include <exception>

Client::Client( ) {
    this->_fd = -1;
    this->_registered = false;
	this->_nickname = "guest";
	this->_username = "guestuser";
	this->_messageCompleted = true;
}

Client::Client( int fd ) {
	this->_nickname = "guest";
	this->_username = "guestuser";
    this->_fd = fd;
    this->_registered = false;
    this->_messageCompleted = true;
}

Client::Client( int fd, std::string address ) {
    this->_fd = fd;
	this->_nickname = "guest";
	this->_username = "guestuser";
    this->_address = address;
    this->_registered = false;
    this->_messageCompleted = true;
}

Client::Client( int fd, std::string nickname, std::string username, std::string address ) {
	this->_fd = fd;
	this->_nickname = nickname;
	this->_username = username;
	this->_address = address;
	this->_registered = false;
	this->_messageCompleted = true;
}

LEVEL Client::getAuthLevel( void ) const {
	return this->_level;
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

void Client::SetAuthLevel( int level ) {
	this->_level = LEVEL(level);
}

void    Client::SwitchToRegistered( void ) {
    this->_registered = true;
}

bool    Client::IsRegistered( void ) const {
    return (this->_registered);
}


void Client::QueueMessage(const std::string& msg) {
	this->_messages.push(msg);
}

bool Client::HasMessages( void ) const {
	return (!this->_messages.empty());
}

std::string Client::GetNextMessage( void ) {
	if (!this->_messages.empty()) {
		std::string message = this->_messages.front();
		this->_messages.pop();
		return message;
	}
	return "";
}

void Client::SendMessage( const std::string& msg ) const {
	Server::SendMessage(this->_fd, msg);
}

void Client::StoreBuffer( char *str, int size ) {
    if (str == NULL || size == 0)
        return ;
    this->_buffer += std::string(str);
    if (str[size - 1] != '\n') {
        this->_messageCompleted = false;
    } else {
        this->_messageCompleted = true;
    }
}

bool Client::IsBufferReady( void ) const {
    return this->_messageCompleted;
}

std::string Client::GetBuffer( void ) {
    std::string tmp(_buffer);
    this->_messageCompleted = true;
    this->_buffer.clear();
    return tmp;
}

bool	passCmd(Server& irc_srv, Client& clt, std::vector<std::string>& args)
{
	try {
		if (clt.getAuthLevel() != LEVEL(0))
			throw std::runtime_error(
				ERR_ALREADYREGISTERED(clt.GetNickname())
			);
		if ( ((irc_srv.GetPassword()).size() != args[1].size()) || \
			(memcmp(irc_srv.GetPassword().c_str(), args[1].c_str(), irc_srv.GetPassword().length()) != 0))
			throw std::runtime_error(
				ERR_PASSWDMISMATCH(clt.GetNickname())
			);
		clt.SetAuthLevel(1);
	} catch (const std::exception& e) {
		clt.SendMessage(e.what());
		return false;
	}
}

/* TODO:
	* Nicknames are non-empty strings with the following restrictions:
	* They MUST NOT contain any of the following characters: space (' ', 0x20), comma (',', 0x2C), asterisk ('*', 0x2A), question mark ('?', 0x3F), exclamation mark ('!', 0x21), at sign ('@', 0x40). ✅
	* They MUST NOT start with any of the following characters: dollar ('$', 0x24), colon (':', 0x3A). ✅
	* They MUST NOT start with a character listed as a channel type, channel membership prefix, or prefix listed in the IRCv3 multi-prefix Extension. ❌ [not all are implemented]
	* They SHOULD NOT contain any dot character ('.', 0x2E). ✅
*/

bool CheckNickname( std::string nickname ) {

	if (nickname == "" || nickname[0] == '$' || nickname[0] == ':' ||
		nickname[0] == '&' || nickname[0] == '#' || isdigit(nickname[0])) {
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

bool findNickNameMatch(std::string &param, std::map<int, Client> &clients)
{
	std::map<int, Client>::const_iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.GetNickname() == param)
			return false;
	}
	return true;
}


bool	nickCmd(Server& irc_srv, Client& clt, std::map<int, Client> &clients, std::vector<std::string>& args)
{
	try {
		if (!args[1].c_str() || args[1].empty())
			throw std::runtime_error(
				ERR_NONICKNAMEGIVEN(clt.GetNickname())
			);
		if (!findNickNameMatch(args[1], clients))
			throw std::runtime_error(
				ERR_NICKNAMEINUSE(clt.GetNickname(), clt.GetNickname())
			);
		if (!CheckNickname(args[1]))
			throw std::runtime_error(
				ERR_NICKNAMEINUSE(clt.GetNickname(), clt.GetNickname())
			);
		clt.SetNickname(args[1]);
		clt.SetAuthLevel(1);
		return true;
	} catch (const std::exception& e) {
		clt.SendMessage(e.what());
		return false;
	}
}
