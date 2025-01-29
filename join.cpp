#include "Channel.hpp"
#include <vector>

//JOIN "#channel1,#channel2,#channel3 key1,key2"

// bool    acceptUser(Channel& ch, std::string _user)
// {

// }

// void    addChannel(std::vector<Channel>& ch_list,
//     const std::pair<std::string, std::string>& ch_key, std::string _user) {

    


// }

int    channelExiste(std::vector<Channel>& ch_list, const std::string& channel)
{
    for (int i = 0; i < ch_list.size(); i++) {
        if (ch_list[i].getName() == channel) {
            std::cout<<"found channel\n";
            return (i);
        }
    }
    return (-1);
}

bool    joinChannel(std::vector<Channel>& ch_list, const std::string& channel, const std::string& key, const std::string& user)
{
    if (channel[0] != '#' && channel[0] != '&')
    {

        return false;
    }
    int i = channelExiste(ch_list, channel);
    if (i != -1)
    {
        Channel& targetCH = ch_list[i];
        if (targetCH.getKeyProtected() && targetCH.getKey() != key) {
            //wrong password
            return false;
        }
        if (targetCH.getLimit() != std::string::npos &&
            targetCH.getLimit() < targetCH.getUserNum() + 1) {
            //no place left
            return false;
        }
        if (targetCH.isInviteOnly() && !targetCH.isUserWelcomed(user))
        {
            //not welcomed
            return false;
        }

        targetCH.addMember(user);
        //check password;
        //check limit;
        //check if is invited;
    }
    else {
        Channel new_ch(channel, key);
        new_ch.addMember(user);
        new_ch.upgradeOp(user);
        ch_list.push_back(new_ch);
    }
    return true;
    
}

bool    joinCmd(std::vector<Channel>& ch_list, std::string args, std::string _user) {
    std::cout<<"starting...\n";
    std::vector<std::string> channels;
    std::vector<std::string> keys;
    std::string channels_list;
    std::string key_list;

    size_t space_pos = args.find(' ');
    if (space_pos != std::string::npos)
    {
        channels_list = args.substr(0, space_pos);
        key_list = args.substr(space_pos + 1);
    }
    else
        channels_list = args;

    size_t pos = 0;
    while ((pos = channels_list.find(",")) != std::string::npos) {
        std::string new_ch = channels_list.substr(0, pos);
        if (!new_ch.empty())
            channels.push_back(new_ch);
        channels_list.erase(0, pos + 1);
    }
    if (!channels_list.empty())
        channels.push_back(channels_list);

    pos = 0;
    while ((pos = key_list.find(",")) != std::string::npos) {
        std::string new_key = key_list.substr(0, pos);
        if (!new_key.empty())
            keys.push_back(new_key);
        key_list.erase(0, pos + 1);
    }
    if (!key_list.empty())
        keys.push_back(key_list);

    for (int i = 0; i < channels.size(); i++) {
        std::string channel_key = (i < keys.size()) ? keys[i] : "";
        if(joinChannel(ch_list, channels[i], channel_key, _user))
            std::cout<<"JOIN SUCCESSFULY!!\n";
    }
    std::cout<<"done!\n";
    return true;     
}