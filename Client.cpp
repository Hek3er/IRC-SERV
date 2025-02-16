#include "Client.hpp"
#include "Server.hpp"
#include <exception>

Client::Client( ) {
    this->_fd = -1;
    this->_registered = false;
	this->_nickname = "";
	this->_username = "";
	this->_messageCompleted = false;
}

Client::Client( int fd ) {
	this->_nickname = "";
	this->_username = "";
    this->_fd = fd;
    this->_registered = false;
    this->_messageCompleted = false;
}

Client::Client( int fd, std::string address ) {
    this->_fd = fd;
	this->_nickname = "";
	this->_username = "";
    this->_address = address;
    this->_registered = false;
    this->_messageCompleted = false;
}

Client::Client( int fd, std::string nickname, std::string username, std::string address ) {
	this->_fd = fd;
	this->_nickname = nickname;
	this->_username = username;
	this->_address = address;
	this->_registered = false;
	this->_messageCompleted = false;
}

LEVEL Client::getAuthLevel( void ) const {
	return this->_level;
}

// LEVEL Client::getAuthLevel( void ) const {
// 	return this->_level;
// }

std::string	Client::GetNickname( void ) const {
	return (this->_nickname);
}

std::string Client::GetUsername( void ) const {
	return (this->_username);
}

std::string Client::GetAddress( void ) const {
	return (this->_address);
}

std::string Client::getRealName( void ) const {
	return (this->_realname);
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

void	Client::SetRealname( std::string realname ) {
	this->_realname = realname;
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
    if (str == NULL || size == 0 || str[0] == '\n' || str[0] == '\r') {
		this->_messageCompleted = false;
	    return ;
	}

	for (int i = 0; i < size; i++) {
		if (str[i] != 4) {
			this->_buffer += str[i];
		}
	}

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

//PASS

bool	passCmd(Server& irc_srv, Client& clt, std::vector<std::string>& args)
{
	try {
		if (clt.getAuthLevel() != LEVEL(0))
			throw std::runtime_error(
				ERR_ALREADYREGISTERED(clt.GetNickname())
			);
		if (args.size() == 1)
			throw std::runtime_error(
				ERR_PASSWDMISMATCH(clt.GetNickname())
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

//NICK

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

std::string boldGreen(const std::string &str) {
	return std::string("\033[1;32m") + str + "\033[0m";
}

bool	nickCmd(Server& irc_srv, Client& clt, std::map<int, Client> &clients, std::vector<std::string>& args)
{
	try {
		bool alreadyseted = false;
		if (args.size() == 1)
			throw std::runtime_error(
				ERR_NONICKNAMEGIVEN(clt.GetNickname())
			);
		if (!findNickNameMatch(args[1], clients))
			throw std::runtime_error(
				ERR_NICKNAMEINUSE(clt.GetNickname(), clt.GetNickname())
			);
		if (!CheckNickname(args[1]))
			throw std::runtime_error(
				ERR_ERRONEUSNICKNAME(clt.GetNickname(), clt.GetNickname())
			);
		if (clt.GetNickname() != "")
			alreadyseted = true;
		if (clt.getAuthLevel() == LEVEL(1))
			clt.SetAuthLevel(2);
		if (clt.getAuthLevel() == LEVEL(3))
			irc_srv.broadcastNick(":" + clt.GetNickname() + "!" + clt.GetUsername() + "@localhost " + boldGreen("NICK") + " " + args[1] + "\r\n");
		else
			std::cout << boldGreen("NICK: ") << args[1] << std::endl;
		clt.SetNickname(args[1]);
		return true;
	} catch (const std::exception& e) {
		clt.SendMessage(e.what());
		return false;
	}
}

// USER

bool checkName(std::string &user_name) {
	const std::string alphaNumeric = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";

    if (user_name.find_first_of(alphaNumeric) == std::string::npos  ) {
		std::cout << "Should be changed" << std::endl;
		return false;
    }
	return true;
}

bool checkconcatRealName(std::string &real_name) {
	if (real_name[0] == ':')
		return true;
	else 
		return false;
}

std::string concatRealName(std::vector<std::string>& args) {
	std::string result;
	std::vector<std::string>::const_iterator it;
	for (it = args.begin() + 4; it != args.end(); it++)
	{
		if (it != args.begin() + 4)
			result += " ";
		result += *it;
	}
	return result.substr(1);
}

bool checkArgs(Client& clt, std::vector<std::string>& args) {
	std::map<int, int> checks;
	checks[0] = 0;
	checks[1] = 0;
	checks[2] = 0;
	int	count = 0;
	if (args[1].empty() || args[2].empty() || args[3].empty() || args[4].empty())
		return false;
	if (!checkName(args[1]))
		checks[0] = 1;
	if (!checkName(args[4]))
		checks[1] = 1;
	if (checkconcatRealName(args[4]))
		checks[2] = 1;
	if (memcmp("0", args[2].c_str(), args[2].length()) != 0)
		return false;
	if (memcmp("*", args[3].c_str(), args[3].length()) != 0)
		return false;
	
	if (checks[0])
		clt.SetUsername("~" + clt.GetNickname());
	else
		clt.SetUsername("~" + args[1]);
	if (checks[1])
		clt.SetRealname(clt.GetNickname());
	else
		clt.SetRealname(args[4]);
	if (checks[2])
		clt.SetRealname(concatRealName(args));
	return true;
}

bool	userCmd(Server& irc_srv, Client& clt, std::vector<std::string>& args)
{
	try {
		if (clt.getAuthLevel() == LEVEL(3))
			throw std::runtime_error(
				ERR_ALREADYREGISTERED(clt.GetNickname())
			);
		if (args.size() < 5)
			throw std::runtime_error(
				ERR_NEEDMOREPARAMS(clt.GetNickname(), "USER")
			);
		if (!checkArgs(clt, args))
			throw std::runtime_error(
				ERR_NEEDMOREPARAMS(clt.GetNickname(), "USER")
			);
		if (clt.getAuthLevel() == LEVEL(1))
			clt.SetAuthLevel(2);
		else
			clt.SetAuthLevel(3);
		std::cout << boldGreen("USER: ") << clt.GetUsername() << " 0 * " << clt.getRealName() << std::endl;
		irc_srv.SendMessage(clt.GetFd(), WELCOME_REPLY(clt.GetNickname(), std::string("ad")));
		return true;
	} catch (const std::exception& e) {
		clt.SendMessage(e.what());
		return false;
	}
}

// PRIVMSG

std::string concatMsg(std::vector<std::string>& args) {
	std::string result;
	std::vector<std::string>::const_iterator it;
	for (it = args.begin() + 2; it != args.end(); it++)
	{
		if (it != args.begin() + 2)
			result += " ";
		result += *it;
	}
	return result.substr(1);
}

Client  getClientByNIck(std::string &nick, std::map<int, Client> &clients) {
    std::map<int, Client>::const_iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.GetNickname() == nick)
			return it->second;
	}
	return it->second;
}

bool	isChannel(std::string channelName) {
	if (channelName[0] == '#')
		return true;
	return false;
}

bool	privmsg(Server& irc_srv, Client& clt, std::map<int, Client> &clients, std::vector<std::string>& args) {
	try {
		if (args.size() == 1)
			throw std::runtime_error(
				ERR_NEEDMOREPARAMS(clt.GetNickname(), "PRIVMSG")
			);
        Client	recieverClient;
		// valid nickname, no text
        if (args.size() == 2 && !isChannel(args[1]) && !findNickNameMatch(args[1], clients))
            throw std::runtime_error(
                ERR_NOTEXTTOSEND(clt.GetNickname(), args[0])
            );
		// not a valide nickname and not a valid channel
        if (args.size() >= 2 && !isChannel(args[1]) && findNickNameMatch(args[1], clients))
			throw std::runtime_error(
				ERR_NOSUCHNICK(clt.GetNickname(), args[1])
			);
		Channel *cltChannel = irc_srv.getChannel(args[1]) ;

		// not a valid channel
		if (isChannel(args[1]) && cltChannel == NULL)
			throw std::runtime_error(
                ERR_CANNOTSENDTOCHAN(clt.GetNickname(), args[1])
            );
		// valid channel but not a member
		if (isChannel(args[1]) && cltChannel != NULL && !cltChannel->isMember(clt.GetFd()))
			throw std::runtime_error(
                ERR_CANNOTSENDTOCHAN(clt.GetNickname(), args[1])
            );
		// valid channel and member but no text
		if (args.size() == 2 && isChannel(args[1]) && cltChannel != NULL && cltChannel->isMember(clt.GetFd()))
			throw std::runtime_error(
                ERR_NOTEXTTOSEND(clt.GetNickname(), args[0])
            );

		if (!isChannel(args[1]) && checkconcatRealName(args[2])) {
        	recieverClient = getClientByNIck(args[1], clients);
        	recieverClient.SendMessage(":" + clt.GetNickname() + " PRVMSG " + recieverClient.GetNickname() + " " + concatMsg(args) + "\r\n");
			return true;
		}
		else if (!isChannel(args[1])) {
        	recieverClient = getClientByNIck(args[1], clients);
			recieverClient.SendMessage(":" + clt.GetNickname() + " PRVMSG " + recieverClient.GetNickname() + " " + args[2] + "\r\n");
			return true;
		}
		if (checkconcatRealName(args[2]))
			cltChannel->brodcastMode(irc_srv ,":" + clt.GetNickname() + " PRVMSG " + args[1] + " :" + concatMsg(args) + "\r\n");
		else
			cltChannel->brodcastMode(irc_srv ,":" + clt.GetNickname() + " PRVMSG " + args[1] + " :" + args[2] + "\r\n");
		return true;
    } catch(const std::exception& e) {
        clt.SendMessage(e.what());
        return false;
    }
}