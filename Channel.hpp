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
        int topic_changer;
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

        void    setInvite(bool condition);
        void    setTopicRestriction(bool condition);
        void    setLimitCondition(bool condition);
        void    setlimit(size_t n);
        void    setKey(std::string key);
        void    removeKey();
        void    setTopic(std::string newTopic);
        void    setTopicChanger(int fd);
        
        std::pair<std::string, std::string> getModes();
        std::string getName();
        std::string getTopic();
        std::string getKey();
        size_t  getLimit();
        size_t  getUserNum();
        int getTopicChanger();

        bool isInviteOnly();
        bool isKeyProtected();
        bool isTopicRes();
        bool isLimit();

        bool isUserWelcomed(int fd);

        void printChannelInfo() const;

        void broadcastJoin(Server& server, int joiner_fd);
        void brodcastMode(Server& server, std::string reply);
        void brodcastTopic(Server& server, std::string TopicReply);


};

bool    joinCmd(Server& irc_srv, Client& clt, std::vector<std::string>& args);
void    inviteCmd(Server& server,Client& clt, std::vector<std::string> args);
void    modeCmd(Server& ss, Client &clt, std::vector<std::string>& args);
void    topic_cmd(Server& ss, Client& clt, std::vector<std::string> args);
std::vector<std::string> split(std::string str, char del);