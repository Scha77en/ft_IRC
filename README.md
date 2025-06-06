# ft_IRC

**ft_IRC** is a C++ IRC (Internet Relay Chat) server project that replicates the core functionality of the IRC protocol, allowing multiple users to chat over a network. This project is an educational exercise in network programming, sockets, and protocol handling using C++.

## Features

- Written in C++98
- Handles multiple clients simultaneously
- Implements core IRC commands (see below)
- Modular code structure

## Getting Started

### Prerequisites

- A C++ compiler supporting C++98 (e.g., `g++`, `clang++`)
- GNU Make
- Linux or macOS operating system

### Building the Project

Clone the repository:

```bash
git clone https://github.com/Scha77en/ft_IRC.git
cd ft_IRC
```

Build the server using the provided Makefile:

```bash
make
```

If built successfully, you should see:

```
--------------- server is ready ---------------
```

This will produce an executable called `ircserv` in the project directory.

### Running the Server

To start the IRC server, run:

```bash
./ircserv <port> <password>
```

- `<port>`: The port number the server will listen on (e.g., 6667)
- `<password>`: The password required for clients to connect

**Example:**

```bash
./ircserv 6667 mypassword
```

### Connecting to the Server (Using netcat)

You can test your IRC server using `nc` (netcat):

```bash
nc 127.0.0.1 6667
```

After connecting, you can type IRC commands directly into the terminal.

## Supported Commands

Once connected, you can test the following IRC commands:

- `TOPIC <#channel> <topic>`  
  Set or view the topic for a channel.

- `INVITE <nickname> <#channel>`  
  Invite another user to a channel.

- `MODE <#channel> <mode>`  
  Set or view channel modes.

- `BOT`  
  Interact with the built-in server bot.

- `JOIN <#channel>[,<#channel2>...]`  
  Join one or more channels.

- `PRIVMSG <target> <message>`  
  Send a private message to a user or channel.

- `PART <#channel>[,<#channel2>...]`  
  Leave one or more channels.

- `KICK <#channel> <user>`  
  Remove a user from a channel.

### Example Session

```bash
nc 127.0.0.1 6667
```
Now type (replace values as needed):
```
PASS mypassword
NICK mynickname
USER mynickname 0 * :Real Name
JOIN #general
PRIVMSG #general :Hello, everyone!
```

## Cleaning Up

To remove object files and the executable:

```bash
make fclean
```

## Project Structure

- `IRC/` — Source files
- `Headers/` — Header files
- `OBJ/` — Compiled object files
- `Makefile` — Build instructions

## License

This project is for educational purposes. For other uses, please contact the author.

---

**Repository:** [https://github.com/Scha77en/ft_IRC](https://github.com/Scha77en/ft_IRC)
