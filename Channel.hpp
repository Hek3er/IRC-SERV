#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include "Client.hpp"
#include "Server.hpp"

class Server;
class Client;

class Channel {
    private:
        std::string name;
        std::string topic;
        std::string key;
        size_t      limit;
        bool        invite_only;
        bool        topic_res;
        bool        isKeyProtected;
        std::set<int> clients_fd;          // Store just fds
        std::set<int> operators_fd;
        std::set<int> inviteds_fd;
        // std::set<std::string> operators;
        // std::set<std::string> members;
        // std::set<std::string> invited_users;

    public:
        Channel();
        Channel(std::string channel_name);
        Channel(std::string channel_name, std::string channel_key);
        Channel(const Channel& cpy);
        Channel& operator=(const Channel& other);
        ~Channel();

        void addMember(int fd);
        void    removeMemeber(int fd);
        void addOp(int fd);
        //bool downgradeOp(Client& clt);
        // bool isOp(Client& clt);

        // bool    setKey(std::string nickname, std::string _key);
        // bool    setTopic(std::string nickname, std::string _topic);
        // bool    setLimit(std::string nickname, size_t _limit);
        void    setlimit(size_t n);
        std::string getName();
        std::string getTopic();
        std::string getKey();
        size_t  getLimit();
        size_t  getUserNum();

        bool isInviteOnly();
        bool getKeyProtected();
        bool isTopicRes();

        bool isUserWelcomed(int fd);

        void printChannelInfo() const;

        void broadcastJoin(Server& server, int joiner_fd);


};

bool    joinCmd(Server& irc_srv, Client& clt);
