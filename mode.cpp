#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "replies.hpp"
#include <vector>

//TODO handel relpies and brodcast for each cmd and execute the mods
//Test +o -o operator


bool isMode(char c) {
    return (c == 'i' || c == 't' || c == 'k' || c == 'o' || c == 'l');
}

void    splitModes(std::vector<std::string> &modes,const std::string& args) {
    bool minus;

    minus = (args[0] == '-');
    for (int i = 1; i < args.length(); i++) {
        modes.push_back(std::string(1, minus ? '-' : '+') + args[i]);
    }
}

void    indexParameters(std::vector<std::string> &modes,std::vector<std::string> &parameters, std::vector<std::pair<std::string, std::string> >& mode_par_index) {
    int index = 0;
    for (int i = 0; i < modes.size(); i++) {
        if (modes[i] == "+k" || modes[i] == "+o" || modes[i] == "-o" || modes[i] == "+l")
        {
            if (index < parameters.size())
                mode_par_index.push_back(std::make_pair(modes[i], parameters[index++]));
            else
                mode_par_index.push_back(std::make_pair(modes[i], "")); // -1 missing parameter
        }
        else
            mode_par_index.push_back(std::make_pair(modes[i], "")); // -2 should not have parameter
    }
}

void    getModes(std::vector<std::string> &modes,std::vector<std::string> &parameters, std::vector<std::string> &args) {
    int i = 2;
    if (args[i][0] == '+' || args[i][0] == '-') {
        splitModes(modes, args[i]);
    }
    else {
         splitModes(modes, "+" + args[i]);
    }
    i++;
    for(; i < args.size(); i++) {
        parameters.push_back(args[i]);
    }
}

void    printargs(std::vector<std::pair<std::string, std::string> >& mode_par_index) {
    for (int i = 0; i < mode_par_index.size(); i++) {
        std::cout<< mode_par_index[i].first << "   " << mode_par_index[i].second << "\n";
    }
}

bool    checkForParameter(std::string &parameter, std::string& mode) {
    if ((mode == "+k" || mode == "+o" || mode == "-o" || mode == "+l") && parameter.empty())
        return false;
    return true;
}

bool executeModes(Server& ss,Channel& ch, Client& clt, std::string &parameter, std::string& mode) {
    if (!checkForParameter(parameter, mode)) {
        clt.SendMessage(ERR_NEEDMOREPARAMS(ss.getHostName(), clt.GetUsername(), "MODE"));
        return false;
    }

    if (mode == "+i" || mode == "-i") {
        (mode[0] == '+') ? ch.setInvite(true) : ch.setInvite(false);
        return (true);
    }
    if (mode == "+t" || mode == "-t") {
        (mode[0] == '+') ? ch.setTopicRestriction(true) : ch.setTopicRestriction(false);
        return (true);
    }
    if (mode == "+k" || mode == "-k") {
        (mode[0] == '+') ? ch.setKey(parameter) : ch.removeKey();
        return (true);
    }
    if (mode == "+o" || mode == "-o") {
        Client *target = ss.getClientByNick(parameter);
        if (!target)
            return false ; // Client doesnt exist!
        (mode[0] == '+') ? ch.addOp(target->GetFd()) : ch.removeOp(target->GetFd());
        return (true);
    }
    if (mode == "+l" || mode == "-l") {
        if (mode[0] == '+') {
            if (parameter.empty()) {
                clt.SendMessage(ERR_NEEDMOREPARAMS(ss.getHostName(), clt.GetNickname(), "MODE"));
                return false;
            }
            try {
                size_t limit = std::stoull(parameter);
                ch.setLimitCondition(true);
                ch.setlimit(limit);
            } catch (const std::invalid_argument& e) {
                clt.SendMessage(ERR_INVALIDMODEPARAM(ss.getHostName(), clt.GetNickname(), ch.getName(), mode, parameter));
                return false;
            } catch (const std::out_of_range& e) {
                clt.SendMessage(ERR_INVALIDMODEPARAM(ss.getHostName(), clt.GetNickname(), ch.getName(), mode, parameter));
                return false;
            }
        } 
        else {
            if (!parameter.empty()) {
                clt.SendMessage(ERR_INVALIDMODEPARAM(ss.getHostName(), clt.GetNickname(), ch.getName(), mode, parameter));
                return false;
            }
            ch.setLimitCondition(false);
        }
        return true;
    }
    clt.SendMessage(ERR_UNKNOWNMODE(ss.getHostName(), clt.GetNickname(), mode[1]));
    return false;
}


void    modeCmd(Server& ss, Client &clt, std::vector<std::string>& args) {
    std::vector<std::string> modes;
    std::vector<std::string> parameters;

    std::vector<std::pair<std::string, std::string> > mode_par_index;

    Channel* working_ch;
    Client* target_clt;

    if (args.size() == 1) {
        clt.SendMessage(ERR_NEEDMOREPARAMS(ss.getHostName(), clt.GetUsername(), "MODE"));
        return ;
    }
    working_ch = ss.getChannel(args[1]);
    if (!working_ch) {
        clt.SendMessage(ERR_NOSUCHCHANNEL(ss.getHostName(), clt.GetUsername(), args[1]));
        return; //ERR_NOSUCHCHANNEL 
    }
    if (args.size() == 2) {
        clt.SendMessage(RPL_CHANNELMODEIS(ss.getHostName(), clt.GetNickname(), working_ch->getName(), working_ch->getModes().first, working_ch->getModes().second));
        clt.SendMessage(RPL_CREATIONTIME(ss.getHostName(), clt.GetNickname(), working_ch->getName(), working_ch->getChannelTime()));
        return ;
    }

    if (!working_ch->isOp(clt.GetFd())) {
        clt.SendMessage(ERR_CHANOPRIVSNEEDED(ss.getHostName(), clt.GetUsername(), working_ch->getName()));
        return;
    }
    getModes(modes, parameters, args);
    indexParameters(modes, parameters, mode_par_index);
    printargs(mode_par_index);
    if (modes.size() == 0) {
        return ; // ERR_UMODEUNKNOWNFLAG
    }
    for (int i = 0; i < mode_par_index.size(); i++) {
        if (executeModes(ss, *working_ch, clt, mode_par_index[i].second, mode_par_index[i].first)) {
            std::string mode = mode_par_index[i].first;
            std::string args = mode_par_index[i].second;
            working_ch->brodcastMode(ss, MODE_CHANGE_REPLY(clt.GetNickname(), clt.GetUsername(), ss.getHostName(), working_ch->getName(), mode, args));
        }
        else
            break ;
    }
}