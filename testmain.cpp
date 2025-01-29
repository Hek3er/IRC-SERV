#include "Channel.hpp"


int main() {


    std::vector<Channel> ch_list;
    ch_list.push_back(Channel("#mychannel"));
    ch_list.push_back(Channel("&passwordchannel", "pass"));
    Channel ch("#limitchannel");
    ch.setlimit(0);
    ch_list.push_back(ch);

    for (int i = 0; i < ch_list.size(); i++)
        ch_list[i].printChannelInfo();
    // joinCmd(ch_list, "#mychannel,&passwordchannel dfd,pass", "ahmed");
    // joinCmd(ch_list, "&", "soufaine");
    joinCmd(ch_list, "#limitchannel", "ahmed");
    for (int i = 0; i < ch_list.size(); i++)
        ch_list[i].printChannelInfo();
}