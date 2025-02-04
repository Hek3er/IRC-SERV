#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <vector>


void    splitModes(std::vector<std::string> &modes, std::string& args) {
    bool plus;

    plus = (args[0] == '+');
    for (int i = 1; i < args.length(); i++) {
        modes.push_back(std::string(1, plus ? '+' : '-') + args[i]);
    }
}

void    executeMode(const std::string& mode, const std::string& parametre) {
    if (mode == "-i" || mode == "+i")
    if (mode == "-t" || mode == "+t")
    if (mode == "-k" || mode == "+k")
    if (mode == "-o" || mode == "+o")
    if (mode == "-l" || mode == "+l")
    ;
    
}

void    getModes(std::vector<std::string> &modes,std::vector<std::string> &paramaters, std::vector<std::string> &args) {
    int i = 3;
    for (; i < args.size(); i++) {
        if (args[i][0] == '+' || args[i][0] == '-') {
            splitModes(modes, args[i]);
        } else {
            break;
        }
    }
    for(; i < args.size(); i++) {
        paramaters.push_back(args[i]);
    }
}

void    printModes(std::vector<std::string> modes, std::vector<std::string> paramaters) {
    for (int i = 0; i < modes.size(); i++) {
        std::cout<<modes[i]<< " "<< (i < paramaters.size() ? paramaters[i]: " ") <<"\n";
    }
}

void    modeCmd(Server& ss,int clt_fd, std::vector<std::string>& args) {
    std::vector<std::string> modes;
    std::vector<std::string> paramaters;

    Channel* working_ch;
    Client* clt;

    working_ch = ss.getChannel(args[1]);
    if (!working_ch) {
        std::cout<<args[1]<<" not found\n";
        return; //ERR_NOSUCHCHANNEL 
    }
    clt = ss.getClientByNick(args[2]);
    if (!clt) {
        std::cout<<"clt not found\n";
        return; //ERR_NOSUCHNICK
    }
    if (clt->GetNickname() != args[2]) {
        std::cout<<"user dont match\n";
        return; //ERR_USERSDONTMATCH
    }
    if (args.size() == 3) {
        //show channel info
        //RPL_CHANNELMODEIS
    }
    getModes(modes, paramaters, args);
    if (modes.size() == 0) {
        return ; // ERR_UMODEUNKNOWNFLAG
    }
}