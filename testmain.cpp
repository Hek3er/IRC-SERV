#include "Channel.hpp"


// int main() {


//     std::vector<Channel> ch_list;
//     ch_list.push_back(Channel("#mychannel"));
//     ch_list.push_back(Channel("&passwordchannel", "pass"));
//     Channel ch("#limitchannel");
//     ch.setlimit(0);
//     ch_list.push_back(ch);

//     for (int i = 0; i < ch_list.size(); i++)
//         ch_list[i].printChannelInfo();
//     // joinCmd(ch_list, "#mychannel,&passwordchannel dfd,pass", "ahmed");
//     // joinCmd(ch_list, "&", "soufaine");
//     joinCmd(ch_list, "#limitchannel", "ahmed");
//     for (int i = 0; i < ch_list.size(); i++)
//         ch_list[i].printChannelInfo();
// }

std::vector<std::string> split(std::string str, char del) {
    std::vector<std::string> result;
    std::string temp = "";
    
    for(int i = 0; i < str.length(); i++) {
        if(str[i] != del) {
            temp += str[i];
        }
        else {
            if(!temp.empty()) {
                result.push_back(temp);
                temp = "";
            }
        }
    }
    if(!temp.empty()) {
        result.push_back(temp);
    }
    
    return result;
}

int main() {
    std::vector<std::string> args = split("    JOIN   #CH  ", ' ');
    std::cout << args.size() <<"\n";
    for (int i = 0 ; i < args.size(); i++)
        std::cout<<args[i]<<" " << i <<"\n";
        
}