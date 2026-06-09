*This project has been created as part of the 42 curriculum by gcesar-n, jcosta-b, and dteruya.*

## Description

ft_irc is a custom Internet Relay Chat (IRC) server developed in C++98 as part of the 42 Common Core.

The goal of the project is to recreate the core functionality of an IRC server while complying with the IRC protocol. The server must be capable of handling multiple clients simultaneously, managing channels, processing commands, and maintaining communication between connected users.

Main features include:

* Multiple client connections.
* Password-based authentication.
* Nickname and username registration.
* Channel creation and management.
* Private and channel messages.
* Channel operators.
* Support for the mandatory IRC commands and channel modes.

## Instructions

### Compilation

Clone the repository and compile the project using:

```bash
make
```

Available Makefile rules:

```bash
make
make clean
make fclean
make re
```

### Execution

Run the server with:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 mypassword123
```

* `port`: Port on which the server listens for incoming connections.
* `password`: Password required for client authentication.

### Connecting to the Server

The server should be tested using an existing IRC client. We chose Irssi as our reference client for simplicity.

When using NetCat, after connecting a client must authenticate using:

```text
PASS <password>
NICK <nickname>
USER <username> 0 * :<realname>
```

Example:

```text
PASS mypassword
NICK student42
USER student42 0 * :Student
```

## Supported Commands

Mandatory IRC commands:

```text
PASS
NICK
USER
JOIN
PRIVMSG
KICK
INVITE
TOPIC
MODE
```

Supported channel modes:

```text
+i  Invite-only channel
+t  Topic changes restricted to operators
+k  Channel password
+o  Operator privileges
+l  User limit
```

## Example Usage

Join a channel:

```text
JOIN #42
```

Send a private message:

```text
PRIVMSG nickname :Hello!
```

Invite a user:

```text
INVITE nickname #42
```
## Resources

### Documentation

* RFC 1459 – Internet Relay Chat Protocol
* RFC 2810 – IRC Architecture
* Beej's Guide to Network Programming
* Linux man pages (`poll`, `bind`, `listen`, `accept`, `socket`, etc.)

### AI Usage

AI tools were used as a learning and documentation aid during the project.

They were primarily used for:

* Understanding IRC protocol concepts.
* Reviewing networking theory and socket programming.
* Searching IRC protocol error codes.
* Clarifying C++98 syntax and standard library quirks.
* Helping implement auto-tests.
* Assisting with Markdown syntax.

All design decisions, implementation, debugging, testing, and final code were completed by the project authors.
