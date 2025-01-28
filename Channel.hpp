#pragma once
#include <iostream>
#include <set>

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

        bool addMember(std::string nickname);
        bool removeMemeber(std::string nickname);
        bool upgradeOp(std::string nickname);
        bool downgradeOp(std::string nickname);
        bool isOp(std::string nickname);

        bool    setKey(std::string nickname, std::string _key);
        bool    setTopic(std::string nickname, std::string _topic);

        std::string getName();
        std::string getTopic();


        
        


};

