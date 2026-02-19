# HTTP Server — Contributor Documentation

This document explains every file, every function, every concept, and every design decision in this project. If you want to contribute, read this first. Nothing here assumes prior knowledge of networking or Assembly.

---

## Table of Contents

1. [What This Project Is](#1-what-this-project-is)
2. [Architecture Overview](#2-architecture-overview)
3. [Project Structure](#3-project-structure)
4. [How Header Files Work](#4-how-header-files-work)
5. [Header Files In Detail](#5-header-files-in-detail)
6. [C Source Files In Detail](#6-c-source-files-in-detail)
7. [Assembly Files In Detail](#7-assembly-files-in-detail)
8. [Full Data Flow](#8-full-data-flow)
9. [The Makefile Explained](#9-the-makefile-explained)
10. [How to Build and Test](#10-how-to-build-and-test)
11. [Key Concepts Glossary](#11-key-concepts-glossary)
12. [Common Errors and Fixes](#12-common-errors-and-fixes)

---

## 1. What This Project Is

A minimal HTTP/1.1 server written in **C** and **x86-64 Assembly** for Linux. It accepts TCP connections, reads raw HTTP requests, parses them, routes them to handler functions, and sends back valid HTTP responses.

No external libraries are used. The Assembly files call Linux syscalls directly. The C files handle all logic and structure.

**Why both C and Assembly?**

| Language | Responsibility |
|---|---|
| C | Structs, control flow, parsing logic, routing, response building |
| Assembly | Syscalls (`read`/`write`), string scanning, memory operations |

The Assembly functions are called from C like normal functions. The x86-64 calling convention makes this seamless — arguments go in `rdi`, `rsi`, `rdx`, return value comes back in `rax`, both in C and in Assembly.

---

## 2. Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│                        main.c                           │
│   - Defines route table (array of method+path+handler)  │
│   - Defines handler functions                           │
│   - Creates server_t, calls server_init + server_run    │
└────────────────────────┬────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────┐
│                       server.c                          │
│   - socket() → bind() → listen() → accept() loop        │
│   - Calls asm_read to get raw bytes from client         │
│   - Calls parse_request, then router_dispatch           │
│   - Calls asm_bzero to clear buffer, close(client_fd)   │
└───────┬──────────────────────────┬──────────────────────┘
        │                          │
        ▼                          ▼
┌───────────────┐        ┌─────────────────────────────────┐
│  request.c    │        │           router.c              │
│  Parses raw   │        │  Loops route table              │
│  HTTP bytes   │        │  Matches method + path          │
│  into struct  │        │  Calls handler function pointer │
└───────┬───────┘        └────────────────┬────────────────┘
        │                                 │
        ▼                                 ▼
┌───────────────┐                ┌─────────────────────────┐
│  str.asm      │                │       response.c        │
│  mem.asm      │                │  Builds HTTP response   │
│  String/mem   │                │  Calls asm_write        │
│  utilities    │                └────────────┬────────────┘
└───────────────┘                             │
                                              ▼
                                    ┌─────────────────┐
                                    │    io.asm        │
                                    │  write() syscall │
                                    └─────────────────┘
```

---

## 3. Project Structure

```
http_server/
├── include/
│   ├── server.h        → server_t struct, lifecycle functions, all Assembly externs
│   ├── request.h       → http_request_t struct, http_header_t struct, parse_request()
│   ├── response.h      → http_response_t struct, send_response(), send_200(), send_404()
│   └── router.h        → route_t struct, handler_fn typedef, router_dispatch()
│
├── src/
│   ├── main.c          → entry point, route table, handler functions
│   ├── server.c        → socket(), bind(), listen(), accept() loop
│   ├── request.c       → HTTP request parser using Assembly string utils
│   ├── response.c      → HTTP response builder using asm_write
│   ├── router.c        → route matching using asm_strncmp
│   └── asm/
│       ├── io.asm      → asm_read, asm_write via Linux syscalls
│       ├── str.asm     → asm_strlen, asm_strchr, asm_strncmp, asm_strncpy
│       └── mem.asm     → asm_memset, asm_memcpy, asm_bzero
│
└── Makefile
```

---

## 4. How Header Files Work

Before touching any specific file, understand this: **header files contain no implementation**. They only declare that something exists.

When you write `#include "server.h"` at the top of a `.c` file, the C preprocessor literally copy-pastes the entire content of `server.h` into that file before compilation begins. This is how the compiler learns about types and functions that are defined in other files.

There are three things that belong in a header:

**1. Struct definitions** — shared data structures that multiple files need to use
```c
typedef struct {
    int port;
    int server_fd;
} server_t;
```

**2. Function prototypes** — the signature of a function so the compiler can verify calls are correct before it sees the implementation
```c
int server_init(server_t *srv, int port);
```

**3. Constants and macros** — values that need to be shared
```c
#define MAX_HEADERS 32
#define BUFFER_SIZE 8192
```

The `#ifndef / #define / #endif` wrapper at the top and bottom of every header is called an **include guard**. It prevents the header from being pasted in twice if multiple files include it.

```c
#ifndef SERVER_H   // if SERVER_H has not been defined yet
#define SERVER_H   // define it now (so next time this is skipped)

// ... all your declarations ...

#endif             // end of the guard
```

---

## 5. Header Files In Detail

### `include/server.h`

This is the central header. It defines the server struct and declares every function — both the C functions in `server.c` and all the Assembly functions in the `asm/` folder. Any `.c` file that needs to use a socket, read bytes, or manipulate strings includes this header.

```c
#ifndef SERVER_H
#define SERVER_H

typedef struct {
    int port;
    int backlog;
    int server_fd;
} server_t;

// Lifecycle — implemented in server.c
int  server_init(server_t *srv, int port);
void server_run(server_t *srv);
void server_close(server_t *srv);

// Assembly I/O — implemented in src/asm/io.asm
long asm_read(int fd, void *buf, unsigned long count);
long asm_write(int fd, const void *buf, unsigned long count);

// Assembly string utilities — implemented in src/asm/str.asm
unsigned long  asm_strlen(const char *s);
char          *asm_strchr(const char *s, int c);
int            asm_strncmp(const char *a, const char *b, unsigned long n);
char          *asm_strncpy(char *dst, const char *src, unsigned long n);

// Assembly memory utilities — implemented in src/asm/mem.asm
void asm_memset(void *buf, int c, unsigned long n);
void asm_memcpy(void *dst, const void *src, unsigned long n);
void asm_bzero(void *buf, unsigned long n);

#endif
```

**Why are Assembly functions declared here?**
Because the linker needs to connect the calls in C to the labels in the `.asm` files. Declaring them here tells the C compiler "this function exists somewhere, trust me" — the linker finds it later in the compiled `.o` file from the Assembly.

---

### `include/request.h`

Defines everything needed to represent a parsed HTTP request.

```c
#ifndef REQUEST_H
#define REQUEST_H

#define MAX_HEADERS  32
#define MAX_PATH     512
#define MAX_METHOD   16
#define BUFFER_SIZE  8192

typedef struct {
    char key[128];
    char value[256];
} http_header_t;

typedef struct {
    char          method[MAX_METHOD];
    char          path[MAX_PATH];
    char          version[16];
    http_header_t headers[MAX_HEADERS];
    int           header_count;
    char         *body;
    int           content_length;
} http_request_t;

int parse_request(const char *raw, http_request_t *req);

#endif
```

**Why two structs?**
`http_header_t` is a single key-value pair (one header line). `http_request_t` holds an array of them alongside the method, path, and body. Separating them makes the structure clear and avoids a flat mess of fields.

---

### `include/response.h`

Defines everything needed to build and send an HTTP response.

```c
#ifndef RESPONSE_H
#define RESPONSE_H

typedef struct {
    int         status_code;
    const char *status_text;
    const char *content_type;
    const char *body;
    int         body_length;
} http_response_t;

void send_response(int client_fd, http_response_t *res);
void send_200(int client_fd, const char *body, const char *content_type);
void send_404(int client_fd);

#endif
```

`send_200` and `send_404` are convenience wrappers around `send_response` so handler functions don't have to build the full struct every time.

---

### `include/router.h`

Defines the routing table entry and the function pointer type for handlers.

```c
#ifndef ROUTER_H
#define ROUTER_H

#include "request.h"
#include "response.h"

// A handler is any function that takes a client fd and a parsed request
typedef void (*handler_fn)(int client_fd, http_request_t *req);

typedef struct {
    char       method[16];
    char       path[256];
    handler_fn handler;
} route_t;

void router_dispatch(int client_fd, http_request_t *req,
                     route_t *routes, int route_count);

#endif
```

**What is `handler_fn`?**
It is a **function pointer type**. Instead of storing the name of a function, you store its memory address. This lets `router_dispatch` call whichever handler matches without knowing its name at compile time. Any function that returns `void` and takes `(int, http_request_t*)` qualifies as a `handler_fn`.

---

## 6. C Source Files In Detail

### `src/main.c`

The entry point. It should be kept simple — its only job is to wire everything together.

**What it contains:**

- The actual handler functions (`handle_index`, `handle_about`, etc.)
- The route table — a static array of `route_t` structs
- `main()` which creates a `server_t`, calls `server_init`, and calls `server_run`

**What it must NOT contain:**

- Any socket logic
- Any HTTP parsing logic
- Any raw byte manipulation

**Why define handlers here and not in their own file?**
For a small server this is fine. If the project grows, handlers can be moved to a dedicated `handlers.c` file and declared in `handlers.h`.

---

### `src/server.c`

The most system-level file. Owns everything related to sockets and the connection loop.

#### `server_init(server_t *srv, int port)`

Sets up the server socket step by step:

1. Store `port` and set `backlog = 10` in the struct
2. Call `socket(AF_INET, SOCK_STREAM, 0)` — creates a TCP IPv4 socket. The OS returns a file descriptor. Store it in `srv->server_fd`.
3. Call `setsockopt` with `SO_REUSEADDR` so the port can be reused immediately when the server restarts without waiting ~60 seconds
4. Fill a `sockaddr_in` struct:
   - `sin_family = AF_INET` — IPv4
   - `sin_port = htons(port)` — port in network byte order (big-endian)
   - `sin_addr.s_addr = INADDR_ANY` — accept connections on any network interface
5. Call `bind(srv->server_fd, (struct sockaddr *)&addr, sizeof(addr))` — attach the socket to that address
6. Call `listen(srv->server_fd, srv->backlog)` — mark the socket as passive, ready to accept

Each step checks the return value and returns `-1` with `perror()` if it fails.

**Why `(struct sockaddr *)&addr`?**
`bind()` was designed to work with multiple address types (IPv4, IPv6, Unix sockets). Its second parameter is the generic `struct sockaddr *`. Your `addr` is `struct sockaddr_in`. The cast tells the compiler to treat the pointer as the generic type — the bytes in memory don't change, only how the compiler interprets the type.

#### `server_run(server_t *srv)`

Runs forever with `while(1)`:

1. Declare `struct sockaddr_in client_addr` and `socklen_t client_len = sizeof(client_addr)`
2. Call `accept(srv->server_fd, (struct sockaddr *)&client_addr, &client_len)` — blocks here until a client connects. The OS fills `client_addr` with the client's IP and port. Returns a new `client_fd` for this specific client.
3. Print the client IP using `inet_ntoa(client_addr.sin_addr)`
4. Call `asm_read(client_fd, buffer, BUFFER_SIZE)` to read the raw HTTP request
5. Call `parse_request(buffer, &req)` to parse it
6. Call `router_dispatch(client_fd, &req, routes, route_count)` to handle it
7. Call `asm_bzero(buffer, BUFFER_SIZE)` to clear the buffer so old data doesn't leak into the next request
8. Call `close(client_fd)` — end this connection and free the file descriptor

**Why is `client_len` a pointer in `accept()`?**
`accept()` communicates in both directions. Before the call you tell it how big your struct is. After the call it writes back how many bytes it actually used. It needs a pointer to be able to modify the value.

#### `server_close(server_t *srv)`

Calls `close(srv->server_fd)`. Also called by the `SIGINT` signal handler so `Ctrl+C` releases the port cleanly instead of leaving it occupied.

---

### `src/request.c`

Parses a raw HTTP request buffer into a structured `http_request_t`. Uses the Assembly string utilities for all scanning.

**What a raw HTTP request looks like:**

```
GET /about HTTP/1.1\r\n
Host: localhost:8080\r\n
User-Agent: curl/7.88\r\n
Accept: */*\r\n
\r\n
```

The structure is always:
- **Line 1** — request line: `METHOD PATH VERSION`
- **Lines 2 to N** — headers: `Key: Value` one per line
- **Empty line** — `\r\n\r\n` marks the end of headers
- **Everything after** — the body (only present for POST, PUT, etc.)

#### `parse_request(const char *raw, http_request_t *req)`

Step by step:

1. Use `asm_strchr(raw, ' ')` to find the first space — copy everything before it into `req->method` using `asm_strncpy`
2. Advance past the space. Use `asm_strchr` again to find the second space — copy everything between them into `req->path`
3. Advance past the second space. Use `asm_strchr` to find `\r\n` — copy everything before it into `req->version`
4. Enter a loop: advance line by line using `asm_strchr('\n')`
   - For each line, use `asm_strchr(':')` to find the colon
   - Everything before the colon is the header key
   - Everything after (trimmed) is the header value
   - Store in `req->headers[req->header_count++]`
   - Stop when the line is just `\r\n` — that is the blank line between headers and body
5. Set `req->body` to point at the first byte after the blank line
6. Return `-1` if any required field was not found, `0` on success

---

### `src/response.c`

Builds and sends valid HTTP responses using `asm_write`.

**What a valid HTTP response looks like:**

```
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 20\r\n
\r\n
<h1>Hello World</h1>
```

The rules are strict — the blank line between headers and body is mandatory, and every line must end with `\r\n` not just `\n`.

#### `send_response(int client_fd, http_response_t *res)`

1. Use `snprintf` to build the status line into a local `header_buf`: `"HTTP/1.1 200 OK\r\n"`
2. Append `Content-Type` header
3. Append `Content-Length` header using `res->body_length`
4. Append the blank line `"\r\n"`
5. Call `asm_write(client_fd, header_buf, header_len)` to send the header block
6. Call `asm_write(client_fd, res->body, res->body_length)` to send the body

Two separate `asm_write` calls are used because the header is in a local stack buffer and the body comes from the caller — they are not contiguous in memory.

#### `send_200` and `send_404`

Convenience wrappers that fill in the `http_response_t` struct and call `send_response`. Handler functions use these instead of building the full struct manually every time.

---

### `src/router.c`

Matches a parsed request against the route table and calls the right handler.

#### `router_dispatch(int client_fd, http_request_t *req, route_t *routes, int route_count)`

Loops through the route table from index `0` to `route_count - 1`:

1. Use `asm_strncmp(req->method, routes[i].method, MAX_METHOD)` — compare the request method with the route's method
2. If they match (return value is `0`), use `asm_strncmp(req->path, routes[i].path, MAX_PATH)` — compare the path
3. If both match, call `routes[i].handler(client_fd, req)` and return immediately
4. If the loop ends with no match, call `send_404(client_fd)`

The route table is a simple linear array. The first matching route wins. Order matters — put more specific routes before more general ones.

---

## 7. Assembly Files In Detail

All Assembly files target **x86-64 Linux** and follow the **System V AMD64 calling convention**:

| Register | Role |
|---|---|
| `rdi` | First argument |
| `rsi` | Second argument |
| `rdx` | Third argument |
| `rax` | Return value / syscall number |
| `al` / `sil` | Low 8 bits of `rax` / `rsi` — used for single byte operations |

When C calls an Assembly function, the arguments are already in `rdi`, `rsi`, `rdx` before the first line of your Assembly runs. You never set them yourself. When you `ret`, whatever is in `rax` becomes the return value in C.

---

### `src/asm/io.asm`

Provides `read` and `write` by calling Linux syscalls directly.

```nasm
section .text

global asm_read
global asm_write

asm_read:
    ; rdi = fd, rsi = buf, rdx = count
    mov rax, 0      ; syscall number 0 = read
    syscall         ; kernel executes read, result in rax
    ret

asm_write:
    ; rdi = fd, rsi = buf, rdx = count
    mov rax, 1      ; syscall number 1 = write
    syscall         ; kernel executes write, result in rax
    ret
```

**Why does this work with no extra setup?**
Because the x86-64 calling convention and the Linux syscall convention use the same registers for the first three arguments (`rdi`, `rsi`, `rdx`). The arguments C placed there before the call are exactly where the kernel expects them. You only need to set `rax` to the syscall number and fire `syscall`.

**`global`** — makes the label visible to the linker so C can call it by name.

---

### `src/asm/str.asm`

String scanning utilities called by the HTTP request parser. Every request goes through these functions.

#### `asm_strlen`

```nasm
asm_strlen:
    ; rdi = string pointer
    xor rax, rax              ; counter = 0
.loop:
    cmp byte [rdi + rax], 0   ; is current byte null?
    je  .done                 ; yes → we're done
    inc rax                   ; no → increment counter
    jmp .loop
.done:
    ret                       ; rax = length
```

`xor rax, rax` is the standard way to zero a register in Assembly — it is one byte shorter than `mov rax, 0` and compilers prefer it.

#### `asm_strchr`

Scans a string for a specific byte and returns a pointer to it.

```nasm
asm_strchr:
    ; rdi = string pointer, rsi = character to find
.loop:
    mov al, byte [rdi]    ; load current byte into al
    cmp al, sil           ; compare with target (low byte of rsi)
    je  .found            ; match → return pointer
    cmp al, 0             ; null terminator?
    je  .notfound         ; yes → character not in string
    inc rdi               ; advance pointer
    jmp .loop
.found:
    mov rax, rdi          ; return pointer to matching byte
    ret
.notfound:
    xor rax, rax          ; return NULL (0)
    ret
```

#### `asm_strncmp`

Compares two strings up to `n` bytes. Returns the difference of the first differing bytes, or `0` if equal.

Walk both strings simultaneously. Each iteration: check if the count ran out, load one byte from each, subtract them — if the result is non-zero they differ, return that. If either byte is null, stop. Advance both pointers, decrement count, repeat.

#### `asm_strncpy`

Copies up to `n` bytes from `src` to `dst`, stopping at the null terminator.

Walk `src` byte by byte. Each iteration: check if count is zero. Load one byte from `src`, write it to `dst`. If the byte was null, stop. Advance both pointers, decrement count, repeat.

---

### `src/asm/mem.asm`

Buffer operations for zeroing and copying raw memory.

#### `asm_bzero`

Fills a buffer with zero bytes. Called after every request to prevent data from leaking into the next connection's buffer.

```nasm
asm_bzero:
    ; rdi = buffer, rsi = count
    xor al, al           ; al = 0
.loop:
    test rsi, rsi        ; is count 0?
    je   .done           ; yes → stop
    mov  byte [rdi], al  ; write 0 to current byte
    inc  rdi             ; advance pointer
    dec  rsi             ; decrement count
    jmp  .loop
.done:
    ret
```

`test rsi, rsi` is equivalent to `cmp rsi, 0` but shorter — it performs a bitwise AND and sets flags without changing the value.

#### `asm_memset`

Same as `asm_bzero` but fills with any byte value (in `sil`, the low byte of `rsi`) instead of zero.

#### `asm_memcpy`

Copies `rdx` bytes from `rsi` to `rdi` byte by byte. Each iteration: check if count is zero, load one byte from source, write to destination, advance both pointers, decrement count.

---

## 8. Full Data Flow

Here is exactly what happens from the moment a client connects to the moment a response is sent:

```
Client opens TCP connection to port 8080
            │
            ▼
accept() unblocks in server_run()
Returns client_fd. OS fills client_addr with client IP/port.
            │
            ▼
asm_read(client_fd, buffer, BUFFER_SIZE)
Reads raw HTTP bytes into buffer:
"GET /about HTTP/1.1\r\nHost: localhost\r\n\r\n"
            │
            ▼
parse_request(buffer, &req)
Uses asm_strchr to find spaces and \r\n boundaries.
Fills req.method = "GET", req.path = "/about", req.version = "HTTP/1.1"
Fills req.headers[] with all header key-value pairs.
            │
            ▼
router_dispatch(client_fd, &req, routes, route_count)
Loops route table. Uses asm_strncmp to compare method and path.
Finds matching route_t where method="GET" and path="/about".
Calls routes[i].handler(client_fd, &req)
            │
            ▼
handle_about(client_fd, &req)
Calls send_200(client_fd, "<h1>About</h1>", "text/html")
            │
            ▼
send_response(client_fd, &res)
Builds header string with snprintf:
"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 14\r\n\r\n"
Calls asm_write(client_fd, header_buf, header_len)
Calls asm_write(client_fd, res->body, res->body_length)
            │
            ▼
asm_bzero(buffer, BUFFER_SIZE)    → clear buffer
close(client_fd)                  → end connection
Back to accept()                  → wait for next client
```

---

## 9. The Makefile Explained

```makefile
CC        = gcc
NASM      = nasm
CFLAGS    = -Wall -O2 -Iinclude
NASMFLAGS = -f elf64

ASM_SRCS  = src/asm/io.asm src/asm/str.asm src/asm/mem.asm
C_SRCS    = src/main.c src/server.c src/request.c src/response.c src/router.c

ASM_OBJS  = $(ASM_SRCS:.asm=.o)
C_OBJS    = $(C_SRCS:.c=.o)

server: $(C_OBJS) $(ASM_OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(NASM) $(NASMFLAGS) $< -o $@

clean:
	rm -f $(C_OBJS) $(ASM_OBJS) server
```

| Flag | Meaning |
|---|---|
| `-Wall` | Show all compiler warnings |
| `-O2` | Enable compiler optimizations |
| `-Iinclude` | Look in `include/` for `.h` files |
| `-f elf64` | Produce 64-bit Linux object files from Assembly |
| `-c` | Compile only, do not link (produces `.o` file) |
| `$@` | The target name (the file being built) |
| `$^` | All dependencies listed |
| `$<` | The first dependency (the source file) |

**Build process:**
1. Each `.c` file is compiled into a `.o` object file
2. Each `.asm` file is assembled into a `.o` object file
3. All `.o` files are linked together by `gcc` into the final `server` binary

The linker is what resolves the connection between `asm_write` called in C and the `asm_write` label defined in `io.asm`.

---

## 10. How to Build and Test

**Requirements:** `gcc`, `nasm`, `make` on Linux x86-64

```bash
# Build
make

# Run
./server

# Test in a second terminal
curl http://localhost:8080/
curl http://localhost:8080/about

# Kill if port is stuck after crash
kill -9 $(lsof -t -i :8080)

# Clean build artifacts
make clean
```

---

## 11. Key Concepts Glossary

| Term | Explanation |
|---|---|
| **File descriptor (fd)** | An integer the OS gives you when you open a resource (socket, file, pipe). You pass it to `read`, `write`, `close`. |
| **socket()** | Creates a new socket endpoint. Returns a file descriptor. No connection yet. |
| **bind()** | Attaches the socket to a specific IP + port on your machine. |
| **listen()** | Marks the socket as passive — ready to receive incoming connections. |
| **accept()** | Blocks until a client connects. Returns a new fd for that client only. |
| **sockaddr_in** | Struct holding an IPv4 address and port. Used by `bind()` and `accept()`. |
| **htons()** | Host to Network Short — converts port to big-endian byte order as required by the network stack. |
| **INADDR_ANY** | Accept connections on any network interface of this machine. |
| **SO_REUSEADDR** | Socket option allowing port reuse immediately after the server closes. Without it, you wait ~60 seconds between restarts. |
| **syscall** | A direct call into the Linux kernel. The number in `rax` identifies which syscall. Arguments in `rdi`, `rsi`, `rdx`. |
| **rdi / rsi / rdx** | x86-64 registers for the first, second, third function arguments. |
| **rax** | x86-64 register for syscall numbers and return values. |
| **`\r\n`** | Carriage return + line feed. HTTP requires this as a line terminator — `\n` alone is not valid. |
| **`\r\n\r\n`** | Two CRLF sequences in a row. Marks the boundary between HTTP headers and body. |
| **Function pointer** | A variable storing a function's address. Allows calling a function without knowing its name at compile time. Used in the route table. |
| **typedef** | Creates a type alias. `typedef struct { ... } server_t;` lets you write `server_t` instead of `struct server` everywhere. |
| **include guard** | The `#ifndef / #define / #endif` pattern in headers that prevents a header from being included more than once in the same translation unit. |
| **Calling convention** | The agreed rules between C and Assembly for how arguments and return values are passed through registers. |
| **Linker** | The tool (invoked by `gcc` at the end) that connects compiled object files together, resolving references like `asm_write` in C to the actual label in `io.o`. |

---

## 12. Common Errors and Fixes

| Error | Cause | Fix |
|---|---|---|
| `bind: Address already in use` | Port is still held from a previous run | Run `kill -9 $(lsof -t -i :8080)` or add `SO_REUSEADDR` in `server_init` |
| `server_t` underlined red | Missing `typedef` in `server.h` — wrote `struct { } server_t` instead of `typedef struct { } server_t` | Add `typedef` before `struct` |
| `/n` printing literally | Wrote `/n` instead of `\n` in a `printf` — forward slash vs backslash | Replace `/n` with `\n` |
| Assembly function undefined at link time | Missing `global` declaration in the `.asm` file, or missing `extern` declaration in the `.h` file | Add `global asm_read` in `io.asm` and the prototype in `server.h` |
| `accept()` never returns | `listen()` was not called before `server_run()` | Ensure `listen()` is called at the end of `server_init()` |
| Old request data appears in new request | Buffer not zeroed between connections | Call `asm_bzero(buffer, BUFFER_SIZE)` at the start or end of each loop iteration in `server_run()` |
| Port still occupied after `Ctrl+C` | No `SIGINT` handler — `server_close()` never called | Add `signal(SIGINT, handler)` in `server_init()` that calls `server_close()` and `exit(0)` |
