#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "stdlib.h"
#include "stdio.h"
#include <fcntl.h>


std::map<int, Client> Server::_clients;
std::vector<struct pollfd> Server::_fds;

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

void    Server::broadcastNick(int fd, std::string nickReply) {
    bool LocalSend = true;
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->isMember(fd)) {
            it->brodcastMode(*this, nickReply);
            LocalSend = false;
        }
    }
    if (LocalSend)
        this->SendMessage(fd, nickReply);
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

void Server::RunServer( void ) {
	// ⚠️ Maybe i should throw exceptions when error?

	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // fill my ip address automatically

	int ret = getaddrinfo(NULL, _port.c_str(), &hints, &res);
	if (ret < 0) {
	   std::cerr << "Error : getaddrinfo failed" << std::endl;
		return ;
	}

	// create a socket for the server (it listens for incomming connections only!)
	this->_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (this->_sockfd == -1) {
		std::cerr << "Coudn't create a socket for the server" << std::endl;
		return ;
	}

    if (fcntl(this->_sockfd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "fcntl failed" << std::endl;
    }
    // if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, NULL, 1) == -1) {
    //     std::cerr << "setcockopt failed" << std::endl;
    //     return ;
    // }

	if (bind(this->_sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		std::cerr << "Coudn't bind the address" << std::endl;
		return ;
	}
    if (listen(this->_sockfd, SOMAXCONN) == -1) {
    	std::cerr << "Coudn't listen" << std::endl;
    	return ;
    }


        char ad[1024] = {0};
        gethostname(ad, sizeof(ad));
        hostName = std::string(ad);
        std::cout << "Server Waiting for connections..." << ad << " " << this->_port << std::endl;

        struct pollfd fd;
        memset(&fd, 0, sizeof(fd));

        fd.fd = this->_sockfd;
        fd.events = POLLIN;
        this->_fds.push_back(fd);

        while(1) {
            int client_fd;

           	// i should change the params in accept (rn this is just a test)

            int ret = poll(this->_fds.data(), this->_fds.size(), 1000);
            if (ret == -1) {
                std::cerr << "error in poll" << std::endl;
            }

            for (int i = 0; i < this->_fds.size(); i++) {
                if (this->_fds[i].revents & POLLIN) {
                    if (this->_fds[i].fd == this->_sockfd) {
                        //new client
                        if ((client_fd = accept(this->_sockfd, 0, 0)) == -1) {
                      		std::cout << "Coudn't accept" << std::endl;
                      		// return ; // should be removed because the server shoudn't quit
                       	}
                        if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
                            std::cerr << "fcntl failed" << std::endl;
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
                                std::cerr << boldGreeen("from client [") << std::to_string(this->_fds[i].fd) << boldGreeen("]") + " : " << buffer;
                                
                                std::vector<std::string> args = split(buffer, ' ');
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
    // }

    	/*
    	 * The idea rn is to get the information of the client the nickname then assign it to a client in the map
    	 * since the map has a key of nickname we need to retrive the nickname and the information first;
    	 *
    	 * This is used so we can have instante lookup times when we need to make direct messages
    	*/
	// std::cout << "sockfd => " << this->_sockfd << " Clientfd => " << client_fd << std::endl;
	freeaddrinfo(res);
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
    // should close _fds
	(this->_sockfd == -1) ? : close(this->_sockfd);
}

//added by Soufiane:
std::vector<Channel>& Server::getChannelList() {
    return channels;
}
Client& Server::getClient(int fd) {
    return _clients.at(fd);
}

Channel* Server::getChannel(std::string channelName) {
    for(int i = 0; i < channels.size(); i++) {
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
