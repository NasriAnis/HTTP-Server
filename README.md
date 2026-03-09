# Custom C HTTP Server

A lightweight, zero-dependency HTTP server written in C. This project implements a core HTTP/1.1 engine from scratch, featuring a custom request parser, response builder, and a flexible routing system.

## 🚀 Features

- **Custom Request Parser**: Efficiently parses HTTP methods, paths, and headers using a zero-copy approach.
- **Dynamic Routing**: Easily map URIs to C handler functions.
- **Response Builder**: Programmatically construct HTTP responses with status codes and custom bodies.
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
│   └── server.c        # Socket setup and connection handling
├── Makefile            # Build system
├── DEVELOPMENT_PLAN.md # Roadmap and progress tracking
└── README.md           # This file
```

## 🛠️ Getting Started

### Prerequisites
- GCC compiler
- Make

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

## 🧪 Testing
You can test the endpoints using `curl` or your web browser:
- `curl http://localhost:8080/`
- `curl http://localhost:8080/about`

## 🤝 Contributing

1. Fork the repository
2. Create a new branch: `git checkout -b feature/my-feature`
3. Commit your changes: `git commit -m "Add new feature"`
4. Push to your branch: `git push origin feature/my-feature`
5. Open a Pull Request
