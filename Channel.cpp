#include "Channel.hpp"
#include "replies.hpp"

Channel::Channel() {}

Channel::Channel(std::string channel_name) : name(channel_name) {
    topic = "";
    key = "";
    limit = 0;
    user_limit = false;
    invite_only = false;
    topic_res = false;
    is_Key_Protected = false;
    key = "";
    creation_time = getTime();
    topic_time = "";
}

Channel::Channel(std::string channel_name, std::string channel_key)
    : name(channel_name) {
    topic = "";
    limit = 0;
    user_limit = false;
    invite_only = false;
    topic_res = false;
    is_Key_Protected = true;
    key = channel_key;
    creation_time = getTime();
    topic_time = "";
    }

Channel::Channel(const Channel& cpy) :
    name(cpy.name),
    topic(cpy.topic),
    key(cpy.key),
    limit(cpy.limit),
    user_limit(cpy.user_limit),
    invite_only(cpy.invite_only),
    topic_res(cpy.topic_res),
    is_Key_Protected(cpy.is_Key_Protected),
    creation_time(cpy.creation_time),
    topic_time(cpy.topic_time),
    clients_fd(cpy.clients_fd),
    operators_fd(cpy.operators_fd),
    inviteds_fd(cpy.inviteds_fd) {}

Channel& Channel::operator=(const Channel& other) {
    if (this != &other) {
        name = other.name;
        topic = other.topic;
        key = other.key;
        limit = other.limit;
        user_limit = other.user_limit;
        invite_only = other.invite_only;
        topic_res = other.topic_res;
        is_Key_Protected = other.is_Key_Protected;
        clients_fd = other.clients_fd;
        operators_fd = other.operators_fd;
        inviteds_fd = other.inviteds_fd;
        creation_time = other.creation_time;
        topic_time = other.topic_time;
    }
    return *this;
}

Channel::~Channel() {}


void Channel::addMember(int fd) {
    clients_fd.insert(fd);
}
void Channel::addOp(int fd) {
    if(clients_fd.find(fd) != clients_fd.end())
        operators_fd.insert(fd);
}
void   Channel::addInvite(int fd) {
    inviteds_fd.insert(fd);
}
void Channel::removeMemeber(int fd) {
    clients_fd.erase(fd);
    removeOp(fd);
}
void Channel::removeOp(int fd) {
    operators_fd.erase(fd);
}
bool    Channel::isMember(int fd) {
    return (clients_fd.find(fd) != clients_fd.end());
}
bool    Channel::isOp(int fd) {
    return (operators_fd.find(fd) != operators_fd.end());
}

// bool Channel::addMember(std::string nickname) {
//     return (members.insert(nickname).second);
// }
// bool Channel::removeMemeber(std::string nickname) {
//     return (members.erase(nickname) > 0); 
// }
// bool Channel::upgradeOp(std::string nickname) {
//     if (members.find(nickname) != members.end())
//         return (members.insert(nickname).second);
//     return false;
// }
// bool Channel::downgradeOp(std::string nickname) {
//     return (operators.erase(nickname) > 0);
// }
// bool Channel::isOp(std::string nickname) {
//     return (operators.find(nickname) != operators.end());
// }

std::pair<std::string, std::string> Channel::getModes() {
    std::string modes = "+";
    std::string args = "";
    
    if (is_Key_Protected) {
        modes += "k";
        args += "*";
    }
    if (invite_only) {
        modes += "i";
    }
    if (topic_res) {
        modes += "t";
    }
    if (user_limit) {
        modes += "l";
        if (!args.empty()) args += " ";
        std::stringstream ss(getLimit());
        std::string limit;
        ss >> limit;
        args += limit;
    }
    
    if (modes == "+")
        modes = "";
        
    return std::make_pair(modes, args);
}

std::string Channel::getName() {
    return name;
}
std::string Channel::getTopic() {
    return topic;
}
std::string Channel::getKey() {
    return key;
}
size_t Channel::getLimit() {
    return limit;
}
size_t Channel::getUserNum() {
    return clients_fd.size();
}
int Channel::getTopicChanger() {
    return topic_changer;
}

void    Channel::setInvite(bool condition) {
    invite_only = condition;
}
void    Channel::setTopicRestriction(bool condition) {
    topic_res = condition;
}
void    Channel::setLimitCondition(bool condition) {
    user_limit = condition;
}

void Channel::setlimit(size_t n) {
    limit = n;
}

void    Channel::setKey(std::string key) {
    this->key = key;
    this->is_Key_Protected = true;
}
void    Channel::removeKey() {
    this->is_Key_Protected = false;
}

void Channel::setTopic(std::string newTopic) {
    topic = newTopic;
}

void   Channel::setTopicChanger(int fd) {
    topic_changer = fd;
}

bool Channel::isInviteOnly() {
    return invite_only;
}
bool Channel::isKeyProtected() {
    return is_Key_Protected;
}
bool Channel::isTopicRes() {
    return topic_res;
}
bool Channel::isLimit() {
    return user_limit;
}
bool Channel::isUserWelcomed(int fd) {
    if (!invite_only)
        return true;
    return inviteds_fd.find(fd) != inviteds_fd.end();
}

std::string Channel::addPrefix(Client& clt) {
    if (isOp(clt.GetFd())) {
        return ("@" + clt.GetNickname());
    }
    return clt.GetNickname();
}

void Channel::broadcastJoin(Server& server, int joiner_fd) {
    const Client& joiner = server.getClient(joiner_fd);
    std::string joinreply = JOIN_REPLY(joiner.GetNickname(), joiner.GetUsername(), this->name, server.getHostName());
    std::string list = ":" + server.getHostName() + " 353 " + joiner.GetNickname() + " = " + this->getName() + " :";
    for (std::set<int>::iterator it = clients_fd.begin(); it != clients_fd.end(); ++it) {
        list += addPrefix(server.getClient(*it));
        list += " ";
    }
    list += "\r\n:" + server.getHostName() + " 366 " + joiner.GetNickname() + " " + this->getName() + " :End of NAMES list\r\n";
    for (std::set<int>::iterator it = clients_fd.begin(); it != clients_fd.end(); ++it) {
        int fd = *it;
        server.SendMessage(fd, joinreply);
    }
    server.SendMessage(joiner_fd, list);
}

void    Channel::brodcastMode(Server& server, std::string modeReply) {
    for (std::set<int>::iterator it = clients_fd.begin(); it != clients_fd.end(); ++it) {
        int fd = *it;
        server.SendMessage(fd, modeReply);
    }
}

void    Channel::brodcastMessage(Server& server, int fd, std::string modeReply) {
    for (std::set<int>::iterator it = clients_fd.begin(); it != clients_fd.end(); ++it) {
        if (*it != fd) {
            server.SendMessage(*it, modeReply);
        }
    }
}

void    Channel::brodcastTopic(Server& server, std::string TopicReply) {
    for (std::set<int>::iterator it = clients_fd.begin(); it != clients_fd.end(); ++it) {
        int fd = *it;
        server.SendMessage(fd, TopicReply);
    }
}

void    Channel::setTopicDate(std::string date) {
    topic_time = date;
}

std::string Channel::getTopicDate() {
    return topic_time;
}

std::string Channel::getChannelTime() {
    return creation_time;
}

bool Channel::stillMemebers() {
    if (!clients_fd.empty()) {
        std::cout<<"ba9i 3amra\n";
        return true;
    }
    return false;
}

std::vector<std::string> split(std::string str, char del) {
    std::vector<std::string> result;
    std::string temp = "";
    
    for(size_t i = 0; i < str.length(); i++) {
        if(str[i] != del) {
            if(str[i] != '\r' && str[i] != '\n') {
                temp += str[i];
            }
        }
        else if(!temp.empty()) {
            result.push_back(temp);
            temp = "";
        }
    }
    if(!temp.empty()) {
        result.push_back(temp);
    }
    
    return result;
}



std::string getTime(void)
{
	std::stringstream convert;
	convert << time(NULL);
	return (convert.str());
}

