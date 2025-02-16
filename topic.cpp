#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "replies.hpp"
#include <vector>

std::string    getTopic(std::vector<std::string> args) {
    std::string topic = "";
    if (args.size() > 2) {
        for (size_t i = 2; i < args.size(); i++) {
            topic += args[i];
            if (topic[0] != ':')
                return (topic);
            topic += " ";
        }
    }
    if (topic == ": ")
        topic = "";
    if (topic[0] == ':')
        topic = topic.substr(1); // ADD SPACE !
    return topic;
}


void    topic_cmd(Server& ss, Client& clt, std::vector<std::string> args) {
    if (args.size() == 1) {
        clt.SendMessage(ERR_NEEDMOREPARAMS(clt.GetUsername(), "TOPIC"));
        return ; // Not enough parameters
    }
    Channel* working_ch = NULL;
    Client  topicChangerClt;
    int      topicChangerFd;
    if (args.size() > 1) {
        working_ch = ss.getChannel(args[1]);
        if (!working_ch) {
            clt.SendMessage(ERR_NOSUCHCHANNEL(clt.GetUsername(), args[1]));
            return ;
        }
        if (!working_ch->isMember(clt.GetFd())) {
            clt.SendMessage(ERR_NOTONCHANNEL(clt.GetNickname(), working_ch->getName()));
            return;// ERR_NOTONCHANNEL
        }
    }
    
    if (args.size() == 2) {
        if (working_ch->getTopic().empty()) {
            clt.SendMessage(RPL_NOTOPIC(clt.GetNickname(), working_ch->getName()));//RPL_NOTOPIC
            return;
        }
        else {
            topicChangerFd = working_ch->getTopicChanger();
            topicChangerClt = ss.getClient(topicChangerFd);
            clt.SendMessage(RPL_TOPIC(clt.GetNickname(), working_ch->getName(), working_ch->getTopic()));
            clt.SendMessage(RPL_TOPICWHOTIME(clt.GetNickname(), working_ch->getName(), topicChangerClt.GetNickname(), topicChangerClt.GetUsername(), ss.getHostName()));
            return ; //RPL_TOPIC or RPL_NOTOPIC
        }
    }

    if (args.size() > 2) {
        if (working_ch->isTopicRes() && !working_ch->isOp(clt.GetFd())) {
            clt.SendMessage(ERR_CHANOPRIVSNEEDED(clt.GetUsername(), working_ch->getName()));
            return ;
        }
        working_ch->setTopicChanger(clt.GetFd());
        std::string topic = getTopic(args);
        working_ch->setTopic(topic);
        working_ch->brodcastTopic(ss, TOPIC_CHANGE(clt.GetNickname(), clt.GetUsername(), ss.getHostName(), working_ch->getName(), topic));
        // if (args.size() == 3 && args[2] == ":") {
        //     working_ch->setTopic("");
        //     working_ch->brodcastTopic(ss, TOPIC_CHANGE(clt.GetNickname(), clt.GetUsername(), ss.getHostName(), working_ch->getName(), ""));
        //     return ;
        // }
        // if (args[2] == ":" && !args[3].empty()) {
        //     working_ch->setTopic(args[3]);
        //     working_ch->brodcastTopic(ss, TOPIC_CHANGE(clt.GetNickname(), clt.GetUsername(), ss.getHostName(), working_ch->getName(), args[3]));
        //     return ;
        // }
        // if (args.size() == 3 && args[2][0] == ':') {
        //     working_ch->setTopic(args[2].substr(1));
        //     working_ch->brodcastTopic(ss, TOPIC_CHANGE(clt.GetNickname(), clt.GetUsername(), ss.getHostName(), working_ch->getName(), args[2].substr(1)));
        //     return ;
        // }
        // else {
        //     working_ch->setTopic(args[2]);
        //     working_ch->brodcastTopic(ss, TOPIC_CHANGE(clt.GetNickname(), clt.GetUsername(), ss.getHostName(), working_ch->getName(), args[2]));
        //     return ;
        // }
    }
}