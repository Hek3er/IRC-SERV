#include "Channel.hpp"
#include "replies.hpp"

Channel::Channel() {}

Channel::Channel(std::string channel_name) : name(channel_name) {
    topic = "";
    key = "";
    limit = std::string::npos;
    invite_only = false;
    topic_res = false;
    is_Key_Protected = false;
}

Channel::Channel(std::string channel_name, std::string channel_key)
    : name(channel_name) {
    topic = "";
    limit = std::string::npos;
    invite_only = false;
    topic_res = false;
    if (!channel_key.empty()) {
        is_Key_Protected = true;
        key = channel_key;
    }
    else {
        is_Key_Protected = false;
        key = "";
    }
    }

Channel::Channel(const Channel& cpy) :
    name(cpy.name),
    topic(cpy.topic),
    key(cpy.key),
    limit(cpy.limit),
    invite_only(cpy.invite_only),
    topic_res(cpy.topic_res),
    is_Key_Protected(cpy.is_Key_Protected),
    clients_fd(cpy.clients_fd),
    operators_fd(cpy.operators_fd),
    inviteds_fd(cpy.inviteds_fd)
{
}

Channel& Channel::operator=(const Channel& other) {
    if (this != &other) {
        name = other.name;
        topic = other.topic;
        key = other.key;
        limit = other.limit;
        invite_only = other.invite_only;
        topic_res = other.topic_res;
        is_Key_Protected = other.is_Key_Protected;
        clients_fd = other.clients_fd;
        operators_fd = other.operators_fd;
        inviteds_fd = other.inviteds_fd;
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
void Channel::setTopic(std::string newTopic) {
    topic = newTopic;
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

void Channel::broadcastJoin(Server& server, int joiner_fd) {
    const Client& joiner = server.getClient(joiner_fd);
    std::string joinreply = JOIN_REPLY(joiner.GetNickname(), joiner.GetUsername(), this->name, joiner.GetAddress());
    std::string list = ":servername 353 " + joiner.GetNickname() + " = " + this->getName() + " :";
    for (std::set<int>::iterator it = clients_fd.begin(); it != clients_fd.end(); ++it) {
        list += server.getClient(*it).GetNickname();
        list += " ";
    }
    list += "\r\n:servername 366 " + joiner.GetNickname() + " " + this->getName() + " :End of /NAMES list\r\n";
    for (std::set<int>::iterator it = clients_fd.begin(); it != clients_fd.end(); ++it) {
        int fd = *it;
        server.SendMessage(fd, joinreply);
        server.SendMessage(fd, list);
    }
}

std::vector<std::string> split(std::string str, char del) {
    std::vector<std::string> result;
    std::string temp = "";
    
    for(int i = 0; i < str.length(); i++) {
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

// void Channel::printChannelInfo() const {
//     std::cout << "\n=== Channel Information ===" << std::endl;
//     std::cout << "Name: " << name << std::endl;
    
//     // Topic
//     std::cout << "Topic: " << (topic.empty() ? "No topic set" : topic) << std::endl;
//     std::cout << "Topic Restriction: " << (topic_res ? "Yes" : "No") << std::endl;
    
//     // Security Settings
//     std::cout << "Key Protected: " << (isKeyProtected ? "Yes" : "No") << std::endl;
//     if (isKeyProtected)
//         std::cout << "Key: " << key << std::endl;
    
//     std::cout << "Invite Only: " << (invite_only ? "Yes" : "No") << std::endl;
    
//     // User Limits
//     std::cout << "User Limit: " << (limit > 0 ? std::to_string(limit) : "No limit") << std::endl;
//     std::cout << "Current Users: " << clients_fd.size() << std::endl;
    
//     // Members List
//     std::cout << "\nMembers:" << std::endl;
//     for (const auto& member : clients) {
//         std::cout << "- " << member.GetNickname();
//         std::cout << std::endl;
//     }
    
//     std::cout << "========================\n" << std::endl;
// }