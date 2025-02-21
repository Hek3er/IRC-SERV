#include "Channel.hpp"
#include "Server.hpp"
#include "replies.hpp"
#include <vector>

bool    kickClient(Server& ss, Client& clt, Channel& working_ch ,std::string nick, std::string reason) {
    Client *target = NULL;

    if (!working_ch.isMember(clt.GetFd())) {
        clt.SendMessage(ERR_NOTONCHANNEL(ss.getHostName(), clt.GetNickname(), working_ch.getName()));
        return false;
    }
    target = ss.getClientByNick(nick);
    if (!target) {
        clt.SendMessage(ERR_NOSUCHNICK(ss.getHostName(), clt.GetNickname(), nick));
        return false;
    }
    if (!working_ch.isMember(target->GetFd())) {
        clt.SendMessage(ERR_USERNOTINCHANNEL(ss.getHostName(), clt.GetNickname(), nick, working_ch.getName()));
        return false;
    }
    if (!working_ch.isOp(clt.GetFd())) {
        clt.SendMessage(ERR_CHANOPRIVSNEEDED(ss.getHostName(), clt.GetNickname(), working_ch.getName()));
        return false;
    }
    else {
        working_ch.brodcastTopic(ss, KICK_REPLY(clt.GetNickname(), clt.GetUsername(), ss.getHostName(), working_ch.getName(), target->GetNickname(), (reason.empty() ? target->GetNickname() : reason)));
        working_ch.removeMemeber(target->GetFd());
        //working_ch.removeOp(clt.GetFd());
        if (!working_ch.stillMemebers())
            ss.removeChannle(working_ch.getName());
    }
    return true;
}

std::string getReason(std::vector<std::string> args) {
    std::string reason = "";
    int i = 2;
    for (; i < args.size(); i++) {
        if (args[i] == ":" || args[i][0] == ':') {
            if (args[i][0] == ':')
                break;
            else {
                ++i;
                break;
            }
        }
    }
    for (; i < args.size(); i++) {
        reason += args[i];
        reason += " ";
    }
    return (reason);
}


void    kickCmd(Server& ss, Client& clt, std::vector<std::string> args) {
    Channel* working_ch;
    std::string reason;
    std::vector<std::string> targets;

    if (args.size() < 3) {
        clt.SendMessage(ERR_NEEDMOREPARAMS(ss.getHostName(), clt.GetNickname(), args[0]));
        return ;
    }
    working_ch = ss.getChannel(args[1]);
    if (!working_ch) {
        clt.SendMessage(ERR_NOSUCHCHANNEL(ss.getHostName(), clt.GetNickname(), args[1]));
        return; //ERR_NOSUCHCHANNEL 
    }
    reason  = getReason(args);
    targets = split(args[2], ',');
    for (int i = 0; i < targets.size(); i++) {
        if (!kickClient(ss, clt, *working_ch, targets[i], (reason.size() > 1 ? reason.substr(1) : "")))
            break ;
    }
}