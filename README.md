##### Copyright 2023 Padurariu Sabin

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━┏━━━┓┏━━━┓┏━━━┓┏━━━┓━━━┏━┓┏━┓┏━━━┓━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━┃┏━┓┃┃┏━━┛┃┏━┓┃┗┓┏┓┃━━━┃┃┗┛┃┃┃┏━━┛━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━┃┗━┛┃┃┗━━┓┃┃━┃┃━┃┃┃┃━━━┃┏┓┏┓┃┃┗━━┓━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━┃┏┓┏┛┃┏━━┛┃┗━┛┃━┃┃┃┃━━━┃┃┃┃┃┃┃┏━━┛━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━┃┃┃┗┓┃┗━━┓┃┏━┓┃┏┛┗┛┃━━━┃┃┃┃┃┃┃┗━━┓━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━┗┛┗━┛┗━━━┛┗┛━┗┛┗━━━┛━━━┗┛┗┛┗┛┗━━━┛━━━━━━━━━━━━━━━━━━━━━━━
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━


## **General Overview**
---

A server-client chat architecture written entirely in C usign only POSIX
libraries and with minimal need of dynamic memory. The chat was designed to be 
easy to use, maintain and very portable hence the use of a TUI.

## **Important remarks and notes beforehand:**
---

In order for people outside of local network to connect to the server a port 
forward must be set up on the default gateway of the LAN with the port that the
server will be running on.


## **Functionality**
---

## Server-side

The server will create two log files in the directory it resides, one with
general information about the server's activity and one with all the messages 
that are sent across the server.

The messages are encrypted using a simple xor key just for a light security
measure.

To start the server just run the executable from the terminal like so:
```bash
./server 8192
```

As of now the server supports only one command "closetower" that closes the 
servers and all the other connections.

For more information or help use the "--help" flag.

## CLient-side

The client has a box for printing incoming messages and a input box both
implemented in the terminal.

To start the client and connect to the server just run the executable from the 
terminal like so:
```bash
./client james 81.123.210.3 8192
```
The required arguments are the name, the ip address and the port the server is
running on

As of now the client supports only normal messages and one command "/leave" 
that is used to leave the chat and close the connection.

For more information or help use the "--help" flag.

## **Other remarks and programming details**
---

* The implementation is linux kernel style and the code format is done 
accordingly hence every variable is declared at the start of every function
* Minimal use of dynamic memory

## **Future Updates**
---

* Download and upload files to the server (via FTP protocol)
* more commands for the client
    * ability to see people connected
    * ability to create chat rooms
    * ability to see uploaded files and download them
* more commands for the server
    * ability to see people connected and their ip address
    * ability to see chat rooms
    * ability to see uploaded files
    * ability to see server up time
    * ability to ban/kick clients
* Windows Client coming soon
