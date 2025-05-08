Copyright (c) 2022 Sabin Padurariu

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
━━━┏━━━┓┏━━━┓┏━━━┓━━━━┏┓━━┏┓┏━━━┓┏┓━┏┓━━━━┏━━━┓┏━━━┓┏━━━┓┏━━━┓┏━━┓┏━┓━┏┓┏━━━┓━━━
━━━┃┏━┓┃┃┏━┓┃┃┏━━┛━━━━┃┗┓┏┛┃┃┏━┓┃┃┃━┃┃━━━━┃┏━┓┃┃┏━━┛┃┏━┓┃┗┓┏┓┃┗┫┣┛┃┃┗┓┃┃┃┏━┓┃━━━
━━━┃┃━┃┃┃┗━┛┃┃┗━━┓━━━━┗┓┗┛┏┛┃┃━┃┃┃┃━┃┃━━━━┃┗━┛┃┃┗━━┓┃┃━┃┃━┃┃┃┃━┃┃━┃┏┓┗┛┃┃┃━┗┛━━━
━━━┃┗━┛┃┃┏┓┏┛┃┏━━┛━━━━━┗┓┏┛━┃┃━┃┃┃┃━┃┃━━━━┃┏┓┏┛┃┏━━┛┃┗━┛┃━┃┃┃┃━┃┃━┃┃┗┓┃┃┃┃┏━┓━━━
━━━┃┏━┓┃┃┃┃┗┓┃┗━━┓━━━━━━┃┃━━┃┗━┛┃┃┗━┛┃━━━━┃┃┃┗┓┃┗━━┓┃┏━┓┃┏┛┗┛┃┏┫┣┓┃┃━┃┃┃┃┗┻━┃━━━
━━━┗┛━┗┛┗┛┗━┛┗━━━┛━━━━━━┗┛━━┗━━━┛┗━━━┛━━━━┗┛┗━┛┗━━━┛┗┛━┗┛┗━━━┛┗━━┛┗┛━┗━┛┗━━━┛━━━
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┏━┓┏━┓┏━━━┓━━━━┏━━━┓━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┃┃┗┛┃┃┃┏━━┛━━━━┃┏━┓┃━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┃┏┓┏┓┃┃┗━━┓━━━━┗┛┏┛┃━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┃┃┃┃┃┃┃┏━━┛━━━━━━┃┏┛━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┃┃┃┃┃┃┃┗━━┓━━━━━━┏┓━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┗┛┗┛┗┛┗━━━┛━━━━━━┗┛━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# TTY Chat

## General Overview

A lightweight chat architecture written in C using POSIX libraries.
Designed to be easy to use, very portable and easy to maintain.

## Future Updates

* download and upload files to the server
* client commands
    * see people connected
    * create chat rooms
    * see uploaded media
* server commands
    * see people connected and ip address
    * see chat rooms
    * see uploaded files
    * see server up time
    * ban/kick clients

## Functionality

### Server

The server creates two log files in the directory it resides, one containing
general information about the server's activity and one containing all messages
that are sent across the connections.

The messages are encrypted using a simple xor key for a light security measure.

Currently the server supports only one command, `closetower`, that closes the 
server and all connections.

For more information or help use the "--help" flag.

### Client

The client has a terminal user interface where messages are displayed and an
input prompt for writing messages.

Currently, the client supports only normal messages and one command "/leave" 
that is used to leave the chat and close the connection.

For more information or help use the "--help" flag.

## Usage

To start the server just run the executable:
```bash
user@hostname:~$ server 8192
```

To start the client and connect to the server just run the executable:
```bash
user@hostname:~$ client james 81.123.210.3 8192
```

## Implementation

The asynchronous execution is what lies at the core of the implementation. The
server listens for client connections and fowards traffic in an asynchronous
manner which makes it fast and cheap in terms of resources.

### File descriptor Group

The `fd_group` data structure is the working horse of the system.
It manages connections by leveraging the multiplexing mechanism provided by
the `epoll` kernel data struture in order to achieve fast response times.

By using multiplexing the server is able to listen to multiple connections at
the same time and respond to each one of them.

It stores the `sockets` of the connections in static int array and broadcasts
every message received from the clients.
