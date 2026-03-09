# Custom C HTTP Server

A lightweight, multi-threaded HTTP server written in C. This project implements a core HTTP/1.1 engine from scratch, featuring a custom request parser, response builder, and a flexible routing system.

## 🚀 Features

- **Multi-threaded Architecture**: Uses `pthreads` to handle multiple simultaneous client connections without blocking.
- **Static File Serving**: Automatically serves HTML files from a dedicated `public/` directory.
- **Custom Request Parser**: Efficiently parses HTTP methods, paths, and headers using a zero-copy approach.
- **Dynamic Routing**: Easily map URIs to C handler functions.
- **Zero-Dependency**: Built using only standard C libraries and Linux socket APIs.

## 📁 Project Structure

```text
HTTP-Server/
├── include/
│   ├── request.h       # HTTP Request structures and parser prototypes
│   ├── response.h      # HTTP Response structures and builder prototypes
│   ├── router.h        # Routing engine and handler types
│   └── server.h        # Socket initialization and server loop
├── src/
│   ├── main.c          # Entry point and route registration
│   ├── request.c       # Request parsing logic
│   ├── response.c      # Response rendering logic
│   ├── router.c        # Route matching implementation
│   └── server.c        # Socket setup and multi-threaded connection handling
├── public/             # Static assets (HTML, etc.)
├── Makefile            # Build system (linked with -pthread)
├── DEVELOPMENT_PLAN.md # Roadmap and progress tracking
└── README.md           # This file
```

## 🛠️ Getting Started

### Prerequisites
- GCC compiler
- Make
- POSIX-compliant OS (Linux/macOS)

### Building the Server
To compile the project, run:
```bash
make
```

### Running the Server
To start the server on port 8080:
```bash
make run
```

Once running, you can access the server at `http://localhost:8080`.

## 🌐 Local Network Access

Since the server listens on `INADDR_ANY`, it is accessible from any device on your Local Area Network (LAN).

1.  **Find your Local IP**: Run `hostname -I` in your terminal.
2.  **Connect**: On your phone or another laptop connected to the same Wi-Fi, go to:
    `http://[YOUR_IP]:8080`

## 🤝 Contributing

1. Fork the repository
2. Create a new branch: `git checkout -b feature/my-feature`
3. Commit your changes: `git commit -m "Add new feature"`
4. Push to your branch: `git push origin feature/my-feature`
5. Open a Pull Request
