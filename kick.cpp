#include "Channel.hpp"
#include "Server.hpp"
#include "replies.hpp"
#include <vector>

bool    kickClient(Server& ss, Client& clt, Channel& working_ch ,std::string nick, std::string reason) {
    Client *target = NULL;

    target = ss.getClientByNick(nick);
    if (!target) {
        clt.SendMessage(ERR_NOSUCHNICK(ss.getHostName(), clt.GetNickname(), nick));
        return false;
    }
    if (working_ch.isMember(target->GetFd())) {
        clt.SendMessage(ERR_USERNOTINCHANNEL(ss.getHostName(), clt.GetNickname(), nick, working_ch.getName()));
        return false;
    }
    if (!working_ch.isOp(clt.GetFd())) {
        clt.SendMessage(ERR_CHANOPRIVSNEEDED(ss.getHostName(), clt.GetUsername(), working_ch.getName()));
        return;
    }
    else {
        //remove target from channel
    }
}

std::string getReason(std::vector<std::string> args) {
    std::string reason = "";
    bool add = false;
    for (int i = 0; i < args.size(); i++) {
        if (args[i] == ":" || args[i][0] == ':' && add) {
            if (args[i][0] == ':')
                add = true;
        }
    }
}

bool    kickCmd(Server& ss, Client& clt, std::vector<std::string> args) {
    Channel* working_ch;

    if (args.size() < 3) {
        clt.SendMessage(ERR_NEEDMOREPARAMS(ss.getHostName(), clt.GetNickname(), args[0]));
        return false;
    }
    working_ch = ss.getChannel(args[1]);
    if (!working_ch) {
        clt.SendMessage(ERR_NOSUCHCHANNEL(ss.getHostName(), clt.GetUsername(), args[1]));
        return; //ERR_NOSUCHCHANNEL 
    }
}