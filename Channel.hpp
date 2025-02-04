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
        bool        user_limit;
        bool        invite_only;
        bool        topic_res;
        bool        is_Key_Protected;
        std::set<int> clients_fd;          // Store just fds
        std::set<int> operators_fd;
        std::set<int> inviteds_fd;

    public:
        Channel();
        Channel(std::string channel_name);
        Channel(std::string channel_name, std::string channel_key);
        Channel(const Channel& cpy);
        Channel& operator=(const Channel& other);
        ~Channel();

        void    addMember(int fd);
        void    removeMemeber(int fd);
        void    removeOp(int fd);
        void    addOp(int fd);
        void    addInvite(int fd);
        bool    isMember(int fd);
        bool    isOp(int fd);

        //bool downgradeOp(Client& clt);
        // bool isOp(Client& clt);

        // bool    setKey(std::string nickname, std::string _key);
        // bool    setTopic(std::string nickname, std::string _topic);
        // bool    setLimit(std::string nickname, size_t _limit);
        void    setInvite(bool condition);
        void    setTopicRestriction(bool condition);
        void    setLimitCondition(bool condition);
        void    setlimit(size_t n);
        void    setTopic(std::string newTopic);
        
        std::string getName();
        std::string getTopic();
        std::string getKey();
        size_t  getLimit();
        size_t  getUserNum();

        bool isInviteOnly();
        bool isKeyProtected();
        bool isTopicRes();
        bool isLimit();

        bool isUserWelcomed(int fd);

        void printChannelInfo() const;

        void broadcastJoin(Server& server, int joiner_fd);


};

bool    joinCmd(Server& irc_srv, Client& clt, std::vector<std::string>& args);
void    inviteCmd(Server& server,Client& clt, std::vector<std::string> args);
void    modeCmd(Server& ss,int clt_fd, std::vector<std::string>& args);
std::vector<std::string> split(std::string str, char del);