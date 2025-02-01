#include "Client.hpp"
#include "replies.hpp"

bool    invite(Server& irc_srv, Client& clt, std::string nickname, std::string ch_name) {

    //TODO: handle invite with no args (show list of channels).
    Channel* workingChannel = irc_srv.getChannel(ch_name);
    if (!workingChannel)
        return false; // channel doesnt exist! ERR_NOSUCHCHANNEL

    if (!workingChannel->isMember(clt.GetFd()))
        return false; //not a memeber! ERR_NOTONCHANNEL

    if (!workingChannel->isOp(clt.GetFd()) && workingChannel->isInviteOnly())
        return false; //is invite only mode and the sender is not operator ERR_CHANOPRIVSNEEDED
    
    Client* invited = irc_srv.getClientByNick(nickname);
    if (!invite)
        return false; //client doesnt exist on the server!
    
    if (workingChannel->isMember(invited->GetFd()))
        return false; //client already in the channel ERR_USERONCHANNEL
    
    workingChannel->addInvite(invited->GetFd());
    clt.SendMessage(INVITE_REPLY(clt.GetUsername(), clt.GetAddress(), nickname, ch_name));
    //add the client_fd to the invates_fd
    //send confiramtion (RPL_INVITING) to the inviter
    //send INVITE message, with the issuer as <source>, to the target user
    
}

void    handelInvite(Server& server,Client& clt, std::vector<std::string> args) {
    if (args.size() < 3)
        return ;
    std::string nickname = args[1];
    std::string ch_name = args[2];

    invite(server, clt, nickname, ch_name);
}