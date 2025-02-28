#include "Client.hpp"
#include "Server.hpp"
#include "replies.hpp"
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

void Client::SendMessage( const std::string& msg ) const {
	Server::SendMessage(this->_fd, msg);
}

void Client::StoreBuffer( char *str, int size ) {
    if (str == NULL || size == 0 ) {
		this->_messageCompleted = false;
	    return ;
	}

	for (int i = 0; i < size; i++) {
		if (str[i] != 4) {
			this->_buffer += str[i];
		}
	}
	this->_messageCompleted = true;
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
				ERR_ALREADYREGISTERED(irc_srv.getHostName(), clt.GetNickname())
			);
		if (args.size() == 1)
			throw std::runtime_error(
				ERR_NEEDMOREPARAMS(irc_srv.getHostName(), clt.GetNickname(), args[0])
			);
		if ( ((irc_srv.GetPassword()).size() != args[1].size()) || \
			(memcmp(irc_srv.GetPassword().c_str(), args[1].c_str(), irc_srv.GetPassword().length()) != 0))
			throw std::runtime_error(
				ERR_PASSWDMISMATCH(irc_srv.getHostName(), clt.GetNickname())
			);
		clt.SetAuthLevel(1);
	} catch (const std::exception& e) {
		clt.SendMessage(e.what());
		return false;
	}
	return true;
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

bool checkconcatRealName(std::string &real_name) {
	if (real_name[0] == ':')
		return true;
	else 
		return false;
}



bool	nickCmd(Server& irc_srv, Client& clt, std::map<int, Client> &clients, std::vector<std::string>& args)
{
	try {
		if (args.size() == 1 || args[1] == "")
			throw std::runtime_error(
				ERR_NONICKNAMEGIVEN(irc_srv.getHostName() , clt.GetNickname())
			);
		if (!findNickNameMatch(args[1], clients))
			throw std::runtime_error(
				ERR_NICKNAMEINUSE(irc_srv.getHostName(), clt.GetNickname(), args[1])
			);
		if (!CheckNickname(args[1]))
			throw std::runtime_error(
				ERR_ERRONEUSNICKNAME(irc_srv.getHostName(), clt.GetNickname(), args[1])
			);
		
		if (clt.getAuthLevel() == LEVEL(3))
		{
			clt.SetNickname(args[1]);
			irc_srv.brodcastNick(":" + clt.GetNickname() + "!" + clt.GetUsername() + "@localhost " + "NICK" + " " + args[1] + "\r\n");
		} else if (clt.getAuthLevel() == LEVEL(1)) {
			clt.SetAuthLevel(2);
			clt.SetNickname(args[1]);
		} else if (clt.getAuthLevel() == LEVEL(2) && (clt.GetNickname()).empty()) {
			clt.SetAuthLevel(3);
			clt.SetNickname(args[1]);
			clt.SendMessage(WELCOME_REPLY(fd, clt.GetNickname(), irc_srv.getHostName(), clt.GetUsername(), irc_srv.getHostName()));
		}
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
	int check;
	check = 0;
	if (args[1].empty() || args[2].empty() || args[3].empty() || args[4].empty())
		return false;
	if (checkconcatRealName(args[4]))
		check = 1;
	if (memcmp("0", args[2].c_str(), args[2].length()) != 0)
		return false;
	if (memcmp("*", args[3].c_str(), args[3].length()) != 0)
		return false;

	if (check)
		clt.SetRealname(concatRealName(args));
	return true;
}

bool	userCmd(Server& irc_srv, Client& clt, std::vector<std::string>& args)
{
	try {
		if (clt.getAuthLevel() == LEVEL(3))
			throw std::runtime_error(
				ERR_ALREADYREGISTERED(irc_srv.getHostName(), clt.GetNickname())
			);
		if (args.size() < 5)
			throw std::runtime_error(
				ERR_NEEDMOREPARAMS(irc_srv.getHostName(), clt.GetNickname(), "USER")
			);
		if (!checkArgs(clt, args))
			throw std::runtime_error(
				ERR_NEEDMOREPARAMS(irc_srv.getHostName(), clt.GetNickname(), "USER")
			);
		
		if (clt.getAuthLevel() == LEVEL(1)) {
			clt.SetAuthLevel(2);
			clt.SetUsername("~" + args[1]);
			clt.SetRealname(args[4]);
		} else if (clt.getAuthLevel() == LEVEL(2) && (clt.GetUsername()).empty() && (clt.getRealName()).empty()) {
			clt.SetAuthLevel(3);
			std::stringstream ss(clt.GetFd());
			std::string fd;
			ss >> fd;
			clt.SetUsername("~" + args[1]);
			clt.SetRealname(args[4]);
			clt.SendMessage(WELCOME_REPLY(fd, clt.GetNickname(), irc_srv.getHostName(), clt.GetUsername(), irc_srv.getHostName()));
			return true;
		} else if (clt.getAuthLevel() == LEVEL(2) && !(clt.GetUsername()).empty() && !(clt.getRealName()).empty()) {
			throw std::runtime_error(
				ERR_ALREADYREGISTERED(irc_srv.getHostName(), clt.GetNickname())
			);
		}
		return true;
	} catch (const std::exception& e) {
		clt.SendMessage(e.what());
		return false;
	}
}

// bot

void	bountyBot(Client &client, Client &bot, std::string &name) {
	if (name == "LUFFY" || name == ":LUFFY")
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "Luffy 👊🏻 is worth 3,000,000,000 💰"));
	else if (name == "NAMI" || name == ":NAMI")
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "NAMI 👙 is worth 366,000,000 💰"));
	else if (name == "USSOP" || name == ":USSOP")
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "USSOP 🎯 is worth 500,000,000 💰"));
	else if (name == "SANJI" || name == ":SANJI")
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "SANJI 🦵 is worth 1,032,000,000 💰"));
	else if (name == "ZORO" || name == ":ZORO")
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "ZORO ⚔️ is worth 1,111,000,000 💰"));
	else if (name == "ROBIN" || name == ":ROBIN")
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "ROBIN 👢 is worth 930,000,000 💰"));
	else if (name == "CHOPPER" || name == ":CHOPPER")
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "CHOPPER 🧸ྀི is worth 1,000 💰"));
	else if (name == "FRANKY" || name == ":FRANKY")
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "FRANKY 🤖 is worth 394,000,000 💰"));
	else if (name == "BROOK" || name == ":BROOK")
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "BROOK 💀 is worth 383,000,000 💰"));
	else if (name == "JINBE" || name == ":JINBE")
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "JINBE 🌊 is worth 1,100,000,000 💰"));
	else
		client.SendMessage(PRIVMSG_REPLY(bot.GetNickname(), bot.GetUsername(), "@localhost", client.GetNickname(), "this bot is only for Straw Hat 👒🏴‍☠️☠🍖"));
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
				ERR_NEEDMOREPARAMS(irc_srv.getHostName(), clt.GetNickname(), args[0])
			);

        Client	recieverClient;
        if (args.size() == 2 && !isChannel(args[1]) && !findNickNameMatch(args[1], clients))
            throw std::runtime_error(
                ERR_NOTEXTTOSEND(irc_srv.getHostName(), clt.GetNickname(), args[0])
            );

        if (args.size() >= 2 && !isChannel(args[1]) && findNickNameMatch(args[1], clients))
			throw std::runtime_error(
				ERR_NOSUCHNICK(irc_srv.getHostName(), clt.GetNickname(), args[1])
			);
		Channel *cltChannel = irc_srv.getChannel(args[1]);

		if (args[1][0] == ':')
			args[1] = args[1].substr(1);
		
		if (isChannel(args[1]) && cltChannel == NULL)
			throw std::runtime_error(
                ERR_CANNOTSENDTOCHAN(irc_srv.getHostName(), clt.GetNickname(), args[1])
            );
		
		if (isChannel(args[1]) && cltChannel != NULL && !cltChannel->isMember(clt.GetFd()))
			throw std::runtime_error(
                ERR_CANNOTSENDTOCHAN(irc_srv.getHostName(), clt.GetNickname(), args[1])
            );
		
		if (args.size() == 2 && isChannel(args[1]) && cltChannel != NULL && cltChannel->isMember(clt.GetFd()))
			throw std::runtime_error(
                ERR_NOTEXTTOSEND(irc_srv.getHostName(), clt.GetNickname(), args[0])
            );

		if (!isChannel(args[1]) && checkconcatRealName(args[2])) {
        	recieverClient = getClientByNIck(args[1], clients);
			recieverClient.SendMessage(":" + clt.GetNickname() + "!" + clt.GetUsername() + "@localhost"  +" PRIVMSG " + recieverClient.GetNickname() + " :" + concatMsg(args) + "\r\n");
			if (args[1] == "BOUNTYBOT" || args[1] == ":BOUNTYBOT")
				bountyBot(clt, recieverClient, args[2]);
			return true;
		}
		else if (!isChannel(args[1])) {
        	recieverClient = getClientByNIck(args[1], clients);
			recieverClient.SendMessage(":" + clt.GetNickname() + "!" + clt.GetUsername() + "@localhost"  +" PRIVMSG " + recieverClient.GetNickname() + " :" + args[2] + "\r\n");
			if (args[1] == "BOUNTYBOT")
				bountyBot(clt, recieverClient, args[2]);
			return true;
		}
		if (checkconcatRealName(args[2]))
			cltChannel->brodcastMessage(irc_srv, clt.GetFd() ,":" + clt.GetNickname() + " PRIVMSG " + args[1] + " :" + concatMsg(args) + "\r\n");
		else
			cltChannel->brodcastMessage(irc_srv, clt.GetFd() ,":" + clt.GetNickname() + " PRIVMSG " + args[1] + " :" + args[2] + "\r\n");
		
		
		return true;
    } catch(const std::exception& e) {
        clt.SendMessage(e.what());
        return false;
    }
}