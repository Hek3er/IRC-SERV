#include "Channel.hpp"
#include "Server.hpp"
#include "replies.hpp"
#include <vector>

int    channelExiste(std::vector<Channel>& ch_list, const std::string& channel)
{
    for (size_t i = 0; i < ch_list.size(); i++) {
        if (ch_list[i].getName() == channel) {
            // std::cout<<"found channel\n";
            return (i);
        }
    }
    return (-1);
}

bool    joinChannel(Server& ss, const std::string& channel, const std::string& key, Client& clt)
{
    std::vector<Channel>& ch_list = ss.getChannelList();
    if (channel[0] != '#' && channel[0] != '&')
    {
        clt.SendMessage(ERR_NOSUCHCHANNEL(ss.getHostName(), clt.GetNickname(), channel));
        return false;
    }
    int i = channelExiste(ch_list, channel);
    if (i != -1)
    {
        Channel& targetCH = ch_list[i];
        if (targetCH.isKeyProtected() && targetCH.getKey() != key) {
            clt.SendMessage(ERR_BADCHANNELKEY(ss.getHostName(), clt.GetNickname(), channel));
            return false;
        }
        if (targetCH.isLimit() && targetCH.getLimit() < targetCH.getUserNum() + 1) {
            clt.SendMessage(ERR_CHANNELISFULL(ss.getHostName(), clt.GetNickname(), channel));
            return false;
        }
        if (targetCH.isInviteOnly() && !targetCH.isUserWelcomed(clt.GetFd()))
        {
            clt.SendMessage(ERR_INVITEONLYCHAN(ss.getHostName(), clt.GetNickname(), channel));
            return false;
        }
        if (targetCH.isMember(clt.GetFd()))
            return false;
        
        targetCH.addMember(clt.GetFd());
        targetCH.broadcastJoin(ss, clt.GetFd());
    }
    else {
        Channel new_ch(channel);
        new_ch.addMember(clt.GetFd());
        new_ch.addOp(clt.GetFd());
        ch_list.push_back(new_ch);
        ch_list.back().broadcastJoin(ss, clt.GetFd());
    }
    return true;
    
}

bool    joinCmd(Server& irc_srv, Client& clt, std::vector<std::string>& args) {
    std::vector<std::string> channels;
    std::vector<std::string> keys;

    if (args.size() == 1) {
        clt.SendMessage(ERR_NEEDMOREPARAMS(irc_srv.getHostName(), clt.GetNickname(), args[0]));
        return false;
    }
    
    channels = split(args[1], ','); 
    if (args.size() > 2 && !args[2].empty())
        keys = split(args[2], ',');
    for (size_t i = 0; i < channels.size(); i++) {
        std::string channel_key = (i < keys.size()) ? keys[i] : "";
        joinChannel(irc_srv, channels[i], channel_key, clt);
    }
    return true;     
}