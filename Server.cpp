#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "stdlib.h"
#include "stdio.h"


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

	if (bind(this->_sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		std::cerr << "Coudn't bind the address" << std::endl;
		return ;
	}
    if (listen(this->_sockfd, SOMAXCONN) == -1) {
    	std::cerr << "Coudn't listen" << std::endl;
    	return ;
    }


	for (;;) {
        char ad[1024] = {0};
        gethostname(ad, sizeof(ad));
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
                        struct pollfd fd;
                        memset(&fd, 0, sizeof(fd));

                        fd.fd = client_fd;
                        fd.events = POLLIN;
                        this->_fds.push_back(fd);

                        Client cl(client_fd);
                        this->_clients[client_fd] = cl;
                        std::cout << "Clientfd == " << client_fd << std::endl;

                        SendMessage(client_fd, WELCOME_REPLY(_clients[client_fd].GetNickname(), std::string(ad)));
                    } else {
                        char buff[1024] = {0};
                        int res = recv(this->_fds[i].fd, buff, 1023, 0);
                        if (res < 0) {
                            std::cerr << __FILE__ << " : " <<  __LINE__ << " coudn't read from recv" << std::endl;
                        }
                        
                        this->_clients[this->_fds[i].fd].StoreBuffer(buff, res);
                        
                        // std::cout << "read : " << res << " from buffer: " << buff << std::endl;

                        std::string test(buff);
                        // for (int i = 0; i < test.length() + 1; i++) {
                        //     std::cout << test[i] << " = " << (int)(test[i]) << std::endl;
                        // }
                        std::vector<std::string> args = split(test, ' ');
                        if (test == "hello\n") {
                            this->_clients[this->_fds[i].fd].SendMessage("welcome to the server\n");
                        }

                        if (res == 0) {
                            std::cout << "Client [" << this->_fds[i].fd <<  "] disconnected" << std::endl;
                            close(this->_fds[i].fd);
                            this->_fds.erase(this->_fds.begin() + i);
                            continue;
                        } else if (res <0) {
                            std::cerr << "Coudn't receive message" << std::endl;
                        } else {
                            if (this->_clients[this->_fds[i].fd].IsBufferReady()) {
                                std::cout << _clients[_fds[i].fd].GetUsername() << " fd [ " << this->_fds[i].fd << " ] : " << this->_clients[this->_fds[i].fd].GetBuffer();
                            }
                        }
                        // if (test.length() > 4 && test.substr(0,4).compare("JOIN ")) {
                        //     std::string command = test.substr(5);  // Get everything after "JOIN "
                        //     size_t newline_pos = command.find_first_of("\r\n");
                        //     if (newline_pos != std::string::npos) {
                        //         command = command.substr(0, newline_pos);  // Remove the newline
                        //         }
                        //     joinCmd(*this, _clients[this->_fds[i].fd], command);
                        //     //joinCmd(*this, _clients[this->_fds[i].fd], test.substr(5));
                        // }
                        if (args[0] == "JOIN")
                            joinCmd(*this, _clients[this->_fds[i].fd], args);
                        if (args[0] == "INVITE")
                            inviteCmd(*this, _clients[this->_fds[i].fd], args);
                        if (args[0] == "MODE")
                            modeCmd(*this, _clients[this->_fds[i].fd], args);
                    }
                } else if (this->_fds[i].revents & POLLOUT) {
                    int client = this->_fds[i].fd;

                    if (_clients[client].HasMessages()) {
                        std::string msg = _clients[client].GetNextMessage();
                        if ((send(client, msg.c_str(), msg.length(), 0)) == -1) {
                            std::cerr << "Cound't send message to " << client << std::endl;
                        }
                    } else {
                        this->_fds[i].events &= ~POLLOUT;
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
	}
	// std::cout << "sockfd => " << this->_sockfd << " Clientfd => " << client_fd << std::endl;
	freeaddrinfo(res);
}

void	Server::SendMessage( int client_fd, std::string message )  {
    if (_clients.find(client_fd) != _clients.end()) {
        _clients[client_fd].QueueMessage(message);

        for (size_t i = 0; i < _fds.size(); i++) {
            if (client_fd == _fds[i].fd) {
                _fds[i].events |= POLLOUT;
                break;
            }
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