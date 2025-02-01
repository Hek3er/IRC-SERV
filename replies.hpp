#pragma once

#include <iostream>

#define JOIN_REPLY(nick, username, channelname, ipaddress) ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"
#define INVITE_REPLY(client, ipadress, nickname, channel) ":" + client + "-d@" + ipadress + " INVITE " + nickname + " " + channel + "\r\n"