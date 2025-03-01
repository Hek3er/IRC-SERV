# FT_IRC

An Internet Relay Chat (IRC) server implementation in C++.
![IRC-DEMO](https://fmi4cg1yoq.ufs.sh/f/cVtZuJ9vUYxcls0QFOu6cDg8hOYqa0Letmz4HuIGW1CS2jdr)

## Overview

FT_IRC is a lightweight Internet Relay Chat (IRC) server built from scratch in C++. This project implements the core functionality of the IRC protocol, allowing multiple clients to connect simultaneously, join channels, and communicate with each other in real-time.

## Features

- **Referce Client:** LimeChat is our reference client of choice.
- **Basic IRC Commands:** Supports essential IRC commands including JOIN, PRIVMSG, KICK, INVITE, TOPIC, MODE, PASS, USER, NICK
- **Channel Management:** Create, join, and manage channels with operator privileges
- **Private Messaging:** Send direct messages between users
- **Password Protection:** Secure server access with password authentication
- **Multiple Client Support:** Handle numerous concurrent client connections
- **Error Handling:** Comprehensive error management and response codes
- **RFC Compliance:** Follows [Modern Irc 2812](https://modern.ircdocs.horse/)
- **DCC File Transfer:** allows transerfing files using LimeChat

## About the Project implementation

- Built in C++ with a focus on efficient socket programming
- Non-blocking I/O operations for handling multiple clients
- No external libraries required beyond standard C++ libraries

## Prerequisites

- C++ compiler with C++98 support
- Make
- Unix-based operating system (Linux/macOS)

## Building the Project

```bash
git clone https://github.com/Hek3er/IRC-SERV.git
cd IRC-SERV
make
```

## Usage

```bash
./ircserv <port> <password>
```

Where:
- `<port>` is the port number on which your server will accept connections
- `<password>` is the connection password required for clients

### Example

```bash
./ircserv 6667 secretpassword
```

## Connecting to the Server

You can connect to the server using any standard IRC client:

### Using netcat
```bash
nc localhost 6667
```
Then input:
```
PASS secretpassword
NICK yournickname
USER username 0 * :realname
```

### Using an IRC client
Configure your IRC client with:
- Server: localhost (or your server IP)
- Port: 6667 (or your chosen port)
- Password: secretpassword (or your chosen password)

## Supported Commands

| Command | Description |
|---------|-------------|
| PASS    | Sets a connection password |
| NICK    | Sets or changes your nickname |
| JOIN    | Joins a channel |
| PRIVMSG | Sends a message to a user or channel |
| MODE    | Changes channel or user modes options supported : +-i/t/k/o/l |
| TOPIC   | Sets or retrieves a channel's topic |
| KICK    | Removes a user from a channel |
| INVITE  | Invites a user to a channel |

## References

[Modern Irc](https://modern.ircdocs.horse)

[Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
