#pragma once
#include <iostream>
#include <set>
#include <vector>

class Channel {
    private:
        std::string name;
        std::string topic;
        std::string key;
        size_t      limit;
        bool        invite_only;
        bool        topic_res;
        bool        isKeyProtected;
        std::set<std::string> operators;
        std::set<std::string> members;
        std::set<std::string> invited_users;

    public:
        Channel();
        Channel(std::string channel_name);
        Channel(std::string channel_name, std::string channel_key);
        ~Channel();

        bool addMember(std::string nickname);
        bool removeMemeber(std::string nickname);
        bool upgradeOp(std::string nickname);
        bool downgradeOp(std::string nickname);
        bool isOp(std::string nickname);

        bool    setKey(std::string nickname, std::string _key);
        bool    setTopic(std::string nickname, std::string _topic);
        bool    setLimit(std::string nickname, size_t _limit);
        void    setlimit(size_t n);
        std::string getName();
        std::string getTopic();
        std::string getKey();
        size_t  getLimit();
        size_t  getUserNum();

        bool isInviteOnly();
        bool getKeyProtected();
        bool isTopicRes();

        bool isUserWelcomed(std::string nickname);

        void printChannelInfo() const;


};

bool    joinCmd(std::vector<Channel>& ch_list, std::string args, std::string _user);
