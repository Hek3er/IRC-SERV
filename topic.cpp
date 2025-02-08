#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <vector>

void    topic_cmd(Server& ss, Client& clt, std::vector<std::string> args, std::string command) {
    if (args.size() == 1)
        return ; // Not enough parameters
    Channel* working_ch = ss.getChannel(args[1]);
    if (!working_ch)
        return ;
    if (args.size() == 2) {
        return ; //RPL_TOPIC or RPL_NOTOPIC
    }
    if (!working_ch->isMember(clt.GetFd()))
        return;// ERR_NOTONCHANNEL

    if (args.size() > 2) {
        if (working_ch->isTopicRes() && !working_ch->isOp(clt.GetFd()))
            return ;
        // if (args.size() == 3 && args[2] == ":")
        //     working_ch->setTopic("");
        if (args[2] == ":" && !args[3].empty())
            working_ch->setTopic(args[3]);
        if (args.size() == 3 && args[2][0] == ':')
            working_ch->setTopic(args[2].substr(1));
    }
}