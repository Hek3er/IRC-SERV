#pragma once

#include <iostream>

#define WELCOME_REPLY(nick, ipaddress) ":" + ipaddress + " 001 " + nick + " :Welcome to IRC SERVER " + nick + "!user@host\r\n"
#define JOIN_REPLY(nick, username, channelname, ipaddress) ":" + nick + "!" + username + "@" + ipaddress + " JOIN " + channelname + "\r\n"

#define INVITE_REPLY(client, invited_user, ipadress, nickname, channel) ":" + client + "!" + invited_user + "@" + ipadress + " INVITE " + nickname + " " + channel + "\r\n"

#define MODE_CHANGE_REPLY(nick, user, host, channel, cmd, parameter) ":" + nick + "!" + user + "@" + host + " MODE " + channel + " :" + cmd + " " + parameter + "\r\n"

#define TOPIC_CHANGE(nick, user, host, channel, newTopic)  ":" + nick + "!" + user + "@" + host + " TOPIC " + channel + " :" + newTopic + "\r\n"

#define RPL_CHANNELMODEIS(server, client, channel, modestirng, modes_arguments) ":" + server + " 324 " + client + " " + channel + " " + modestirng + " " + modes_arguments + "\r\n"

//ERRORS
#define RPL_NOTOPIC(server, client, channel) ":" + server + " 331 " + client + " " + channel + " :No topic is set\r\n"
#define RPL_TOPIC(server, client, channel, topic) ":" + server + " 332 " + client + " " + channel + " :" + topic + "\r\n"
#define RPL_TOPICWHOTIME(client, channel, clientChanger, usernameChanger, servername) "333 " + client + " " + channel + " " + clientChanger + "!" + usernameChanger + "@" + servername + "\r\n"
#define ERR_NOSUCHNICK(server, client , nick) ":" + server + " 401 " + client + " " + nick + " :No such nick\r\n"
#define ERR_NOSUCHCHANNEL(server, client, channel) ":" + server + " 403 " + client + " " + channel + " :" + "No such channel\r\n"
#define ERR_NOTONCHANNEL(server, client, channel) ":" + server + " 442 " + client + " " + channel + " :You're not on that channel\r\n"
#define ERR_USERONCHANNEL(server, client, nickname, channel) ":" + server + " 443 " + client + " " + nickname + " " + channel + " :is already on channel\r\n"
#define ERR_NEEDMOREPARAMS(server, client, cmd) ":" + server + " 461 " + client + " " + cmd + " :" + "Not enough parameters\r\n"
#define ERR_UNKNOWNMODE(server, client, modechar) ":" + server + " 472 " + client + " " + modechar + " :" + "is an unknown mode character to me\r\n"
#define ERR_INVITEONLYCHAN(server, client, channel) ":" + server + " 473 " + client + " " + channel + " :" + "Cannot join channel (+i)\r\n"
#define ERR_CHANNELISFULL(server, client, channel) ":" + server + " 471 " + client + " " + channel + " :" + "Cannot join channel (+l)\r\n"
#define ERR_BADCHANNELKEY(server, client, channel) ":" + server + " 475 " + client + " " + channel + " :" + "Cannot join channel (+k)\r\n"
#define ERR_CHANOPRIVSNEEDED(server, client, channel) ":" + server + " 482 " + client + " " + channel + ":You're not channel operator\r\n"
#define RPL_INVITING(server, client, nick, channel) ":" + server + " 341 " + client + " " + nick + " " + channel + "\r\n"

// PASS
#define ERR_ALREADYREGISTERED(client) "462 " + client + " :You may not reregister\r\n"
#define ERR_PASSWDMISMATCH(client) "464 " + client + " :Password incorrect\r\n"

// NICK

#define ERR_ERRONEUSNICKNAME(client, nick) "432 " + client + " " + nick + " :Erroneus nickname\r\n"
#define ERR_NONICKNAMEGIVEN(client) "431 " + client + " :No nickname given\r\n"
#define ERR_NICKNAMEINUSE(client, nick) "433 " + client + " " + nick + " :Nickname is already in use\r\n"
#define ERR_NICKCOLLISION(client, nick) "436 " + client + " " + nick + " :Nickname collision KILL from <user>@<host>\r\n"
#define ERR_INVALIDMODEPARAM(server, client, channel, mode, parameter) ":" + server + " 696 " + client + " " + channel + " " + mode + " " + parameter + " :Invalid mode parameter\r\n"
