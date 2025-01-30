#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string channel_name) : name(channel_name) {
    topic = "";
    key = "";
    limit = std::string::npos;
    invite_only = false;
    topic_res = false;
    isKeyProtected = false;
}

Channel::Channel(std::string channel_name, std::string channel_key)
    : name(channel_name), key(channel_key) {
    topic = "";
    limit = std::string::npos;
    invite_only = false;
    topic_res = false;
    isKeyProtected = true;
    }

Channel::~Channel() {}


void Channel::addMember(int fd) {
    clients_fd.insert(fd);
}
void Channel::addOp(int fd) {
    if(clients_fd.find(fd) != clients_fd.end())
        operators_fd.insert(fd);
}
void Channel::removeMemeber(int fd) {
    clients_fd.erase(fd);
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

void Channel::setlimit(size_t n) {
    limit = n;
}


bool Channel::isInviteOnly() {
    return invite_only;
}
bool Channel::getKeyProtected() {
    return isKeyProtected;
}
bool Channel::isTopicRes() {
    return topic_res;
}

bool Channel::isUserWelcomed(int fd) {
    if (!invite_only)
        return true;
    return inviteds_fd.find(fd) != inviteds_fd.end();
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