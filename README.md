*This project has been created as part of the 42 curriculum by ulfernan & asalguer.*

---

## Description

The goal of our ft_irc project is to build a simple, functional Internet Relay Chat (IRC) server from scratch in C++98, tuned to perform alongside nc and HexChat.

The server is capable of handling multiple concurrent client connections without crashing, hanging, or relying on multithreading. Following the IRC protocol, it supports authentication, channels and several channel functionalities and modes.

This server's main architechture rely on a custom even loop based on poll() and an organized way to manage the different events returned by it, instead of the classic conditional revent tree.

Our focus has been on implementing this project as a library anybody can just run without hesitation, and comprehend easily if needs to be.

### Core Architecture
* **Server & EventManager:** `EventManager` wraps the `poll()` system call to monitor state changes (read/write/error) across all connected sockets. The `Server` iterates through these events, and is in charge of managing the different clients, responses, channels, and memory management.
* **Client:** Represents an active connection and all the parameters relevant to it. Stores fragmented incoming streams until a complete `\r\n` terminated command is received, preventing partial reads from crashing the parser.
* **Channel:** The logic core for group chat functionality. It manages the internal state of a room, including the member list, operator privileges, and dynamic channel modes.
* **Message & Response:** `Message` parses raw client strings into standard IRC components (Command, Parameters, Trailing). `Response` handles the generation of standard RFC-compliant numerical replies and prefix-formatted broadcast messages.

---

## Features & Supported Commands

### Channel Modes
Channel operators are able to modify room behavior in real-time using the `MODE` command:
* **`i` (Invite-only):** Restricts channel entry to users who have been explicitly invited via the `INVITE` command.
* **`t` (Topic protection):** When enabled, only Channel Operators can alter the channel topic using the `TOPIC` command.
* **`k` (Key/Password):** Secures the channel with a password that must be provided during a `JOIN` attempt.
* **`l` (User Limit):** Imposes a hard cap on the maximum number of clients allowed in the channel simultaneously.
* **`o` (Operator Privilege):** Allows existing operators to promote or demote other users to operator status.

### Supported Protocol Commands
* **Connection & Registration:** `CAP` (Dummy negotiation for modern client compatibility), `PASS`, `NICK`, `USER`.
* **Communication:** `PRIVMSG` (Supports direct user-to-user messaging and channel broadcasts).
* **Channel Operations:** `JOIN`, `PART`, `KICK`, `INVITE`, `TOPIC`, `MODE`.
* **Server Pings:** `PING`, `PONG`.

---

## Instructions

### Compilation
The project is built using `make` or `make run` (to run after making with pre-stablished pass and port), and strictly complies with the `-Wall -Wextra -Werror -std=c++98` compiler flags. To compile the server, run the following command at the root of the repository:
```bash
make
```
```bash
make run
```
### Execution
The server requires two arguments to start: an available port to listen on, and a connection password to secure the server. If no args are provided, it will run with default port and password stablished in the main file.

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 my_secure_password
```

Connecting to the Server
Once the server is listening, you can connect using any standard graphical IRC client (such as HexChat, irssi, or WeeChat) by pointing it to 127.0.0.1 on your specified port, using the password provided at launch.

Alternatively, you can test the raw protocol connection using nc (Netcat):

```bash
nc 127.0.0.1 6667
```

(Note: When using Netcat, you must manually send the PASS, NICK, and USER commands to complete the registration handshake).

---

## Resources

### References
* **RFC 2812 (Internet Relay Chat: Client Protocol)**: The definitive standard used to map out the command syntax, numeric reply codes (e.g., 324 RPL_CHANNELMODEIS, 403 ERR_NOSUCHCHANNEL), and expected server behaviors.

### AI Usage
* **Conceptual Guidance**: AI has been used only for consulting purposes over protocol compliance questions, acting as a fast-search engine to help us understand and follow the construction of responses properly. On the other hand, it has been used to find small but hard-to-see bugs in specific functions, understand long unreadable logs from the terminal and general theory. No agent has been used to build any of the code, nor any function, as we believe that agents only works when delegating the coding part entirely to them, as well as ethical concerns.