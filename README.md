*This project has been created as part of the 42 curriculum by ulfernan & asalguer.*

---

## Description

The purpose of this project is to build a fully functional Internet Relay Chat (IRC) server from scratch in C++98. The server, identifying as `ircserv`, is capable of handling multiple concurrent client connections without crashing, hanging, or relying on multithreading. It provides a robust backend for real-time messaging, channel management, and user authentication, strictly adhering to the core mechanics of the IRC protocol.

Rather than relying on simple iterative loops, this server features a custom-built event loop driven by the `poll()` system call. This allows for highly efficient non-blocking I/O multiplexing, ensuring that slow clients or fragmented network packets do not bottleneck the server's execution.

### Core Architecture
The project is modularized into highly specialized classes to ensure clean data flow and strict responsibility segregation:
* **Server & EventManager:** The core orchestrator. `EventManager` wraps the `poll()` system call to monitor state changes (read/write/error) across all connected sockets. The `Server` iterates through these events, delegating I/O operations and command execution.
* **Client:** Represents an active connection. It acts as a buffer, storing fragmented incoming streams until a complete `\r\n` terminated command is received, preventing partial reads from crashing the parser.
* **Channel:** The logic core for group chat functionality. It manages the internal state of a room, including the member list, operator privileges, and dynamic channel modes.
* **Message & Response:** The protocol interpreters. `Message` dissects raw client strings into standard IRC components (Command, Parameters, Trailing). `Response` handles the generation of standard RFC-compliant numerical replies and prefix-formatted broadcast messages.

---

## Features & Supported Commands

### Channel Modes
The server fully implements the standard IRC channel modes, allowing operators to modify room behavior in real-time using the `MODE` command:
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
The project is built using `make` and strictly complies with the `-Wall -Wextra -Werror -std=c++98` compiler flags. To compile the server, run the following command at the root of the repository:
```bash
make
```

### Execution
The server requires two arguments to start: an available port to listen on, and a connection password to secure the server.

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

(Note: When using Netcat, you must manually send the PASS, NICK, and USER commands in that exact order to complete the registration handshake).

---

## Resources
This section outlines the classic references and tools utilized during the development of this project.

### References
* **RFC 2812 (Internet Relay Chat: Client Protocol)**: The definitive standard used to map out the command syntax, numeric reply codes (e.g., 324 RPL_CHANNELMODEIS, 403 ERR_NOSUCHCHANNEL), and expected server behaviors.

* **Beej's Guide to Network Programming:** The primary resource for understanding low-level C socket programming. It was heavily consulted for implementing non-blocking sockets (fcntl), the bind()/listen()/accept() cycle, and building the poll() multiplexer.

### AI Usage
* **Conceptual Guidance:** AI was utilized to clarify complex protocol interactions and standard compliance, specifically regarding the strict parsing rules of the MODE command (handling excess parameters) and the broadcast sequencing for the TOPIC and PART commands.

* **Architecture Validation:** AI assisted in reviewing the logic flow of the command parser and ensuring the custom Response class properly assembled numeric codes and prefixes before flushing the data to the non-blocking sockets.