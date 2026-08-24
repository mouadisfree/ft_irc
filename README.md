*This project has been created as part of the 42 curriculum by mokatfi, ekhallaf.*

# ft_irc

## Description

**ft_irc** is an IRC (Internet Relay Chat) server written from scratch in C++98.

The goal of the project is to implement the server side of the IRC protocol as
described in RFC 1459 / RFC 2812, so that real IRC clients can connect to it,
authenticate, talk to each other privately, and hold group conversations in
channels.

The server is **single-threaded and fully non-blocking**. It handles every
connected client inside one event loop built on a single `select()` call: no
`fork()`, no threads, and no blocking read or write anywhere. A slow or
unresponsive client can never stall the server for anybody else.

Because TCP is a byte stream, a single `recv()` may deliver half a command, or
several commands at once. Each client therefore has its own input buffer where
incoming bytes are accumulated until a complete `\r\n`-terminated message is
available, and its own output buffer that is only flushed when `select()`
reports the socket as ready for writing.

## Features

**Authentication**

| Command | Description |
|---------|-------------|
| `PASS`  | Supplies the server password. Required before registration. |
| `NICK`  | Sets the client's nickname. Duplicates are rejected. |
| `USER`  | Sets username, hostname, servername and real name. |

A client is only considered registered once `PASS`, `NICK` and `USER` have all
succeeded, in that order. Until then, every other command is refused with
`451 ERR_NOTREGISTERED`.

**Channels and messaging**

| Command   | Description |
|-----------|-------------|
| `JOIN`    | Joins a channel, creating it if it does not exist. Accepts a key. |
| `PART`    | Leaves a channel, with an optional reason. |
| `PRIVMSG` | Sends a message to a user or to a channel. |
| `TOPIC`   | Views or sets a channel topic. |
| `KICK`    | Removes a user from a channel (operator only). |
| `INVITE`  | Invites a user to a channel. |
| `MODE`    | Sets or clears channel modes (operator only). |
| `QUIT`    | Closes the session; the channels are notified. |
| `PING`    | Connection keep-alive; the server replies with `PONG`. |
| `WHOIS`   | Minimal implementation. |
| `CAP`     | Accepted so that real clients can negotiate and move on. |

The first user to join a channel becomes its operator. If an operator leaves,
operator status is passed to a remaining member, and a channel that becomes
empty is destroyed.

**Channel modes**

| Mode | Description |
|------|-------------|
| `+i` / `-i` | Invite-only. Only invited users may join. |
| `+t` / `-t` | Only operators may change the topic. |
| `+k` / `-k` | Sets or removes a channel key (password). |
| `+o` / `-o` | Grants or revokes operator status for a user. |
| `+l` / `-l` | Sets or removes a limit on the number of members. |

## Instructions

### Project structure

```
ft_irc/
├── include/            headers: Client, Channel, Server, protocol replies
│   ├── client.hpp
│   ├── channel.hpp
│   ├── server.hpp
│   └── irc.hpp
├── src/
│   ├── main.cpp        entry point
│   ├── server/          networking: socket setup, the select() event loop,
│   │                    client lifecycle, message queueing, command dispatch
│   └── commands/         IRC command implementations: PASS, NICK, USER, JOIN,
│                        PART, KICK, INVITE, TOPIC, MODE, PRIVMSG, the Channel class
├── Makefile
└── README.md
```

Every function lives in its own file, named after the function it contains
(for example `src/server/runServerEventLoop.cpp` defines
`Server::runServerEventLoop`). The Makefile picks up every `.cpp` file under
`src/server/` and `src/commands/` automatically via wildcard, so no source
list needs to be maintained by hand.

### Requirements

- A C++ compiler supporting C++98 (`c++` / `g++` / `clang++`)
- `make`
- A Unix-like system (Linux or macOS)

### Compilation

```bash
make
```

This produces an executable named `ircserv`. The project is compiled with
`-Wall -Wextra -Werror -std=c++98`.

Other rules:

```bash
make clean    # remove the executable
make fclean   # full clean
make re       # rebuild from scratch
```

### Execution

```bash
./ircserv <port> <password>
```

- `<port>` — the port the server listens on (1–65535)
- `<password>` — the password clients must supply with `PASS`

Example:

```bash
./ircserv 6667 mypassword
```

### Connecting

With a real IRC client, for example `irssi`:

```bash
irssi -c 127.0.0.1 -p 6667 -w mypassword
```

Or manually with `nc`, typing the registration sequence by hand:

```bash
nc 127.0.0.1 6667
```

```text
PASS mypassword
NICK alice
USER alice 0 * :Alice Example
JOIN #general
PRIVMSG #general :hello everyone
```

The server answers the registration with the standard welcome numerics
(`001`–`004`), after which the client is free to use any command.

## Resources

### Documentation and references

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/) — a readable,
  up-to-date reference for numerics and message format
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — sockets,
  `bind`, `listen`, `accept`, byte order
- `man` pages: `socket(2)`, `bind(2)`, `listen(2)`, `accept(2)`, `select(2)`,
  `recv(2)`, `send(2)`, `fcntl(2)`
- [Valgrind documentation](https://valgrind.org/docs/manual/manual.html) — used
  to check for memory leaks and invalid memory accesses

### Use of AI

AI (Claude) was used as a **reviewer and debugging assistant**, not as the
author of the project's design. Specifically, it was used for:

- **Explaining concepts** — sockets, TCP as a byte stream, `select()` and I/O
  multiplexing, non-blocking I/O, and the structure of the IRC message format.
- **Auditing the project against the evaluation criteria** — running the server
  under test clients and under Valgrind to find crashes, blocking behaviour and
  memory errors that were not visible from reading the code.
- **Diagnosing and fixing specific defects that the audit revealed**, mainly:
  - making all sockets non-blocking and only reading/writing after `select()`
    reports readiness, including handling `EAGAIN`/`EWOULDBLOCK`
  - adding a per-client output buffer so a slow client cannot block the server
  - rewriting the message parser to correctly handle the trailing parameter
    (`:real name with spaces`)
  - removing clients from every channel before their object is destroyed, which
    fixed a use-after-free
  - adding parameter and channel-existence checks to the command handlers,
    which fixed several crashes on malformed input
- **Writing this README.**

All AI-suggested changes were reviewed and tested before being kept.
