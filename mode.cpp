#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "replies.hpp"
#include <vector>

//TODO handel relpies and brodcast for each cmd and execute the mods


bool isMode(char c) {
    return (c == 'i' || c == 't' || c == 'k' || c == 'o' || c == 'l');
}

void    splitModes(std::vector<std::string> &modes,const std::string& args) {
    bool plus;

    plus = (args[0] == '+');
    for (int i = 1; i < args.length(); i++) {
        modes.push_back(std::string(1, plus ? '+' : '-') + args[i]);
    }
}

void    indexParameters(std::vector<std::string> &modes,std::vector<std::string> &paramaters, std::vector<std::pair<std::string, int> >& mode_par_index) {
    int index = 0;
    for (int i = 0; i < modes.size(); i++) {
        if (modes[i] == "+k" || modes[i] == "+o" || modes[i] == "-o" || modes[i] == "+l")
        {
            if (index < paramaters.size())
                mode_par_index.push_back(std::make_pair(modes[i], index++));
            else
                mode_par_index.push_back(std::make_pair(modes[i], -1));
        }
        else
            mode_par_index.push_back(std::make_pair(modes[i], -2));
    }
}

void    getModes(std::vector<std::string> &modes,std::vector<std::string> &paramaters, std::vector<std::string> &args) {
    int i = 3;
    if (args[i][0] == '+' || args[i][0] == '-') {
        splitModes(modes, args[i]);
    }
    else if (isMode(args[i][0])){
         splitModes(modes, "-" + args[i]);
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

bool executeModes(Channel& ch, Client& clt, std::vector<std::string> &paramaters, std::pair<std::string, int>& mode_par_index) {
    if (mode_par_index.first == "+i" || mode_par_index.first == "-i")
    {
        (mode_par_index.first[0] == '+') ? ch.setInvite(true) : ch.setInvite(false);
    }
    if (mode_par_index.first == "+t" || mode_par_index.first == "-i")
    if (mode_par_index.first == "+k" || mode_par_index.first == "-k")
    if (mode_par_index.first == "+o" || mode_par_index.first == "-o")
    if (mode_par_index.first == "+l" || mode_par_index.first == "-l")
    ;
}

void    modeCmd(Server& ss, Client &clt, std::vector<std::string>& args) {
    std::vector<std::string> modes;
    std::vector<std::string> paramaters;

    std::vector<std::pair<std::string, int> > mode_par_index;

    Channel* working_ch;
    Client* target_clt;

    working_ch = ss.getChannel(args[1]);
    if (!working_ch) {
        clt.SendMessage(ERR_NOSUCHCHANNEL(clt.GetUsername(), args[1]));
        return; //ERR_NOSUCHCHANNEL 
    }
    if (args.size() == 2) {
        //show channel info
        //RPL_CHANNELMODEIS
    }

    // target_clt = ss.getClientByNick(args[2]);
    // if (!target_clt) {
    //     std::cout<<"target_clt not found\n";
    //     return; //ERR_NOSUCHNICK
    // }
    // if (target_clt->GetNickname() != args[2]) {
    //     std::cout<<"user dont match\n";
    //     return; //ERR_USERSDONTMATCH
    // }
    getModes(modes, paramaters, args);
    indexParameters(modes, paramaters, mode_par_index);
    if (modes.size() == 0) {
        return ; // ERR_UMODEUNKNOWNFLAG
    }
    printModes(modes, paramaters);
}