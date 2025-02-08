#pragma once

#include <iostream>

#define JOIN_REPLY(nick, username, channelname, ipaddress) ":" + nick + "!~" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"

#define INVITE_REPLY(client, invited_user, ipadress, nickname, channel) ":" + client + "!" + invited_user + "@" + ipadress + " INVITE " + nickname + " " + channel + "\r\n"

#define MODE_CHANGE_REPLY(nick, user, host, channel, cmd) ":" + nick + "!" + username + "@" + host + " MODE " + channel + " " + cmd + "\r\n"
//ERRORS

#define ERR_NOSUCHNICK(client , nick) "401" + client + " " + nick + ":No such nick\r\n"
#define ERR_NOSUCHCHANNEL(client, channel) "403 " + client + " " + channel + " :" + "No such channel\r\n"
#define ERR_NOTONCHANNEL(client, channel) "442" + client + " " + channel + " :You're not on that channel\r\n"
#define ERR_USERONCHANNEL(client, nickname, channel) "443" + client + " " + nickname + " " + channel + " :is already on channel\r\n"
#define ERR_NEEDMOREPARAMS(client, cmd) "461 " + client + " " + cmd + " :" + "Not enough parameters\r\n"
#define ERR_INVITEONLYCHAN(client, channel) "473 " + client + " " + channel + " :" + "Cannot join channel (+i)\r\n"
#define ERR_CHANNELISFULL(client, channel) "471 " + client + " " + channel + " :" + "Cannot join channel (+l)\r\n"
#define ERR_BADCHANNELKEY(client, channel) "475 " + client + " " + channel + " :" + "Cannot join channel (+k)\r\n"
#define ERR_CHANOPRIVSNEEDED(client, channel) "482" + client + " " + channel + ":You're not channel operator\r\n"
#define RPL_INVITING(client, nick, channel) "341" + client + " " + nick + " " + channel