#include "Client.hpp"
#include "replies.hpp"

bool    invite(Server& irc_srv, Client& clt, std::string nickname, std::string ch_name) {

    //TODO: handle invite with no args (show list of channels).
    Channel* workingChannel = irc_srv.getChannel(ch_name);
    if (!workingChannel)
    {
        clt.SendMessage(ERR_NOSUCHCHANNEL(clt.GetUsername(), ch_name));
        return false; // channel doesnt exist! ERR_NOSUCHCHANNEL
    }

    if (!workingChannel->isMember(clt.GetFd()))
    {
        clt.SendMessage(ERR_NOTONCHANNEL(clt.GetUsername(), ch_name));
        return false; //not a memeber! ERR_NOTONCHANNEL
    }

    if (!workingChannel->isOp(clt.GetFd()) && workingChannel->isInviteOnly())
    {
        clt.SendMessage(ERR_CHANOPRIVSNEEDED(clt.GetUsername(), ch_name));
        return false; //is invite only mode and the sender is not operator ERR_CHANOPRIVSNEEDED
    }
    
    Client* invited = irc_srv.getClientByNick(nickname);
    if (!invited) {
        clt.SendMessage(ERR_NOSUCHNICK(clt.GetUsername(), nickname));
        return false; //client doesnt exist on the server!
    }
    
    if (workingChannel->isMember(invited->GetFd())) {
        clt.SendMessage(ERR_USERONCHANNEL(clt.GetUsername(), invited->GetNickname(), ch_name));
        return false; //client already in the channel ERR_USERONCHANNEL
    }
    
    workingChannel->addInvite(invited->GetFd());
    //add the client_fd to the invates_fd
    workingChannel->addInvite(invited->GetFd());
    //send confiramtion (RPL_INVITING) to the inviter
    clt.SendMessage(RPL_INVITING(clt.GetUsername(), nickname, ch_name));
    //send INVITE message, with the issuer as <source>, to the target user
    invited->SendMessage(INVITE_REPLY(clt.GetUsername(),invited->GetUsername(), clt.GetAddress(), nickname, ch_name));
    return true;
}

void    inviteCmd(Server& server,Client& clt, std::vector<std::string> args) {
    if (args.size() < 3) {
        clt.SendMessage(ERR_NEEDMOREPARAMS(clt.GetUsername(), "INVITE"));
            return;
    }
    std::string nickname = args[1];
    std::string ch_name = args[2];

    invite(server, clt, nickname, ch_name);
}