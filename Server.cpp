#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

std::map<int, Client> Server::_clients;
std::vector<struct pollfd> Server::_fds;
bool stop = false;

void    handle_sig(int sig) {
    if (sig == SIGINT) {
        stop = true;
    }
}

Server::Server( std::string port, std::string password ) {
	this->_port = port;
	this->_sockfd = -1;
	this->_password = password;
}

std::string	Server::GetPort( void ) const {
	return (this->_port);
}

std::string Server::GetPassword( void ) const {
	return (this->_password);
}

std::vector<struct pollfd>& Server::getClientsFds( void ) {
    return (this->_fds);
}

void    Server::brodcastNick(std::string modeReply) {
    for (std::vector<struct pollfd>::iterator it = (this->getClientsFds()).begin() + 3; it != (this->getClientsFds()).end(); ++it) {
        int fd = (*it).fd;
        this->SendMessage(fd, modeReply);
    }
}

std::string boldGreeen(const std::string &str) {
	return std::string("\033[1;32m") + str + "\033[0m";
}

void Server::bleachClient(int fd) {
    for (int i = channels.size() - 1; i >= 0; i--) {
        channels[i].removeMemeber(fd);
        if (!channels[i].stillMemebers())
            removeChannle(channels[i].getName());
    }
}

void	Server::removeChannle(std::string name) {
    for (int i = channels.size() - 1; i >= 0; i--) {
        if (channels[i].getName() == name) {
            channels.erase(channels.begin() + i);
            std::cout << "channel removed\n";
            break ;
        }
    }
}

std::vector<std::string> split_new_line(std::string buffer) {
    std::vector<std::string> result;
    char *tock = strtok((char *)buffer.c_str(), "\r\n");
    while (tock  != NULL) {
        std::string tmp = std::string(tock);
        result.push_back(tmp);
        tock = strtok(NULL, "\r\n");
    }
    return result;
}

std::vector<std::string> split_space(std::string buffer) {
    std::vector<std::string> result;
    size_t pos;
    while ((pos = buffer.find(' ')) != std::string::npos) {
        std::string tmp = buffer.substr(0, pos);
        result.push_back(tmp);
        buffer.erase(0, pos + 1);
    }

    if (!buffer.empty()) {
        result.push_back(buffer);
    }
    return result;
}

void Server::RunServer( void ) {

    signal(SIGINT, handle_sig);

	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int ret = getaddrinfo(NULL, _port.c_str(), &hints, &res);
	if (ret < 0) {
	   std::cerr << "Error : getaddrinfo failed" << std::endl;
		return ;
	}

	this->_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (this->_sockfd == -1) {
		std::cerr << "Coudn't create a socket for the server" << std::endl;
        freeaddrinfo(res);
		return ;
	}

    if (fcntl(this->_sockfd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "fcntl failed" << std::endl;
        freeaddrinfo(res);
        return ;
    }
    
    int opt = 1;
    setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(this->_sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		std::cerr << "Coudn't bind the address" << std::endl;
        freeaddrinfo(res);
		return ;
	}
    if (listen(this->_sockfd, SOMAXCONN) == -1) {
    	std::cerr << "Coudn't listen" << std::endl;
        freeaddrinfo(res);
    	return ;
    }

    char ad[1024] = {0};
    gethostname(ad, sizeof(ad));
    hostName = std::string(ad);
    std::cout << "Server Waiting for connections..." << ad << " " << this->_port << std::endl;
    std::cout << "Press ^C to quit" << std::endl;

    struct pollfd fd;
    memset(&fd, 0, sizeof(fd));

    fd.fd = this->_sockfd;
    fd.events = POLLIN;
    this->_fds.push_back(fd);

    int sockfdBot = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfdBot == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        freeaddrinfo(res);
        return;
    }


    int portInt;
    std::stringstream portstream(this->GetPort());
    portstream >> portInt;
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(portInt);
    if (inet_pton(AF_INET ,"127.0.0.1", &serveraddr.sin_addr) <= 0) {
        std::cerr << "Failed to set address to bot sockaddr" << std::endl;
        freeaddrinfo(res);
        close(sockfdBot);
        return ;
    }

    if (connect(sockfdBot, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        std::cerr << "Failed to connect the BOUNTYBOT" << std::endl;
        freeaddrinfo(res);
        close(sockfdBot);
        return ;
    }
    
    if (fcntl(sockfdBot, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "fcntl failed" << std::endl;
        close(sockfdBot);
        freeaddrinfo(res);
        return ;
    }

    Client bot;

    bot.SetFd(sockfdBot);
    bot.SetNickname("BOUNTYBOT");
    bot.SetUsername("BOUNTYBOT");
    bot.SetRealname("BOUNTYBOT");
    bot.SetAddress("");
    bot.SetAuthLevel(3);
    this->_clients[sockfdBot] = bot;

    struct pollfd botfd;
    memset(&botfd, 0, sizeof(botfd));
    botfd.fd = sockfdBot;
    botfd.events = POLLIN;
    this->_fds.push_back(botfd);

    while(stop == false) {
        int client_fd;

        int ret = poll(this->_fds.data(), this->_fds.size(), -1);
        if (ret == -1 && !stop) {
            std::cerr << "error in poll" << std::endl;
            continue;
        }

        for (size_t i = 0; i < this->_fds.size(); i++) {
            if (this->_fds[i].revents & POLLIN) {
                if (this->_fds[i].fd == this->_sockfd) {
                    if ((client_fd = accept(this->_sockfd, 0, 0)) == -1 && !stop) {
                        std::cerr << "Coudn't accept" << std::endl;
                        continue;
                    }
                    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1 && !stop) {
                        std::cerr << "fcntl failed" << std::endl;
                        continue;
                    }
                    struct pollfd fd;
                    memset(&fd, 0, sizeof(fd));

                    fd.fd = client_fd;
                    fd.events = POLLIN;
                    this->_fds.push_back(fd);

                    Client cl(client_fd);
                    this->_clients[client_fd] = cl;
                    _clients[client_fd].SetAuthLevel(0);
                } else {
                    char buff[1024] = {0};
                    int res = recv(this->_fds[i].fd, buff, 1023, 0);
                    if (res < 0) {
                        std::cerr << __FILE__ << " : " <<  __LINE__ << " coudn't read from recv" << std::endl;
                    }

                    this->_clients[this->_fds[i].fd].StoreBuffer(buff, res);

                    if (res == 0) {
                        std::cout << "Client [" << this->_fds[i].fd <<  "] disconnected" << std::endl;
                        close(this->_fds[i].fd);
                        bleachClient(this->_fds[i].fd);
                        this->_clients.erase(this->_fds[i].fd);
                        this->_fds.erase(this->_fds.begin() + i);
                        continue;
                    } else if (res <0) {
                        std::cerr << "Coudn't receive message" << std::endl;
                    } else {
                        if (this->_clients[this->_fds[i].fd].IsBufferReady()) {
                            std::string buffer(this->_clients[this->_fds[i].fd].GetBuffer());
                                std::cout << boldGreeen("from client [") << this->_fds[i].fd << boldGreeen("]") + " : " << buffer;
                            std::vector<std::string> strs = split_new_line(buffer);
                            for (size_t j = 0; j < strs.size(); j++) {
                                std::vector<std::string> args = split_space(strs[j]);
                                if (args.size() == 0) {
                                    continue;
                                }
                                if (args[0] == "PASS")
                                    passCmd(*this, _clients[this->_fds[i].fd], args);
                                if (args[0] == "NICK" && _clients[this->_fds[i].fd].getAuthLevel() >= LEVEL(1))
                                    nickCmd(*this, _clients[this->_fds[i].fd], this->_clients, args);
                                if (args[0] == "USER" && _clients[this->_fds[i].fd].getAuthLevel() >= LEVEL(1))
                                    userCmd(*this, _clients[this->_fds[i].fd], args);
                                if (args[0] == "JOIN" && _clients[this->_fds[i].fd].getAuthLevel() == LEVEL(3))
                                    joinCmd(*this, _clients[this->_fds[i].fd], args);
                                if (args[0] == "INVITE" && _clients[this->_fds[i].fd].getAuthLevel() == LEVEL(3))
                                    inviteCmd(*this, _clients[this->_fds[i].fd], args);
                                if (args[0] == "MODE" && _clients[this->_fds[i].fd].getAuthLevel() == LEVEL(3))
                                    modeCmd(*this, _clients[this->_fds[i].fd], args);
                                if (args[0] == "TOPIC" && _clients[this->_fds[i].fd].getAuthLevel() == LEVEL(3))
                                    topic_cmd(*this, _clients[this->_fds[i].fd], args);
                                if (args[0] == "KICK" && _clients[this->_fds[i].fd].getAuthLevel() == LEVEL(3))
                                    kickCmd(*this, _clients[this->_fds[i].fd], args);
                                if (args[0] == "PRIVMSG" && _clients[this->_fds[i].fd].getAuthLevel() == LEVEL(3))
                                    privmsg(*this, _clients[this->_fds[i].fd], this->_clients, args);
                            }
                        }
                    }
                }
            }
        }
    }
    if (res != NULL) {
	    freeaddrinfo(res);
    }
    close(sockfdBot);
}
void	Server::SendMessage( int client_fd, std::string message )  {
    if (_clients.find(client_fd) != _clients.end()) {
        if ((send(client_fd, message.c_str(), message.length(), 0)) == -1) {
            std::cerr << "Cound't send message to " << client_fd << std::endl;
        }
    } else {
        std::cerr << "Client Not found fd : " << client_fd << std::endl;
    }
}

Server::~Server() {

    std::cout << std::endl <<  "Server is closed" << std::endl;
	(this->_sockfd == -1) ? : close(this->_sockfd);
    for (size_t i = 0; i < this->_fds.size(); i++) {
        if (this->_fds[i].fd != -1) {
            close(this->_fds[i].fd);
        }
    }
}

std::vector<Channel>& Server::getChannelList() {
    return channels;
}
Client& Server::getClient(int fd) {
    return _clients.at(fd);
}

Channel* Server::getChannel(std::string channelName) {
    for(size_t i = 0; i < channels.size(); i++) {
        if (channels[i].getName() == channelName)
            return &channels[i];
    }
    return NULL;
}

Client*	Server::getClientByNick(std::string nick) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.GetNickname() == nick) {
            return &(it->second);
        }
    }
    return NULL;
}

std::string Server::getHostName() {
    return hostName;
}
void	Server::setHostName(std::string name) {
    hostName = name;
}
