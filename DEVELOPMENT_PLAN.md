# HTTP Server Development Plan

This document outlines the step-by-step process to complete the HTTP server project.

## Phase 1: Infrastructure & Data Structures (The Skeleton)
*Goal: Establish the build system and define how data flows through the server.*

- [ ] **1.1 Makefile Implementation**: Create a robust `Makefile` that compiles all `.c` files in `src/`, links them, and outputs an executable. Include `all`, `clean`, and `run` targets.
- [ ] **1.2 Define HTTP Request Structure (`include/request.h`)**:
    - Define `http_method_t` (GET, POST, etc.).
    - Define `http_request_t` struct: `method`, `path`, `version`, and a list for `headers`.
- [ ] **1.3 Define HTTP Response Structure (`include/response.h`)**:
    - Define `http_response_t` struct: `status_code`, `status_message`, `content_type`, `body`, and `body_len`.

## Phase 2: Request Parsing (The Input)
*Goal: Convert raw bytes from the network into a structured C object.*

- [ ] **2.1 Socket Reading**: Implement logic in `src/request.c` to read bytes from the client socket into a heap-allocated buffer.
- [ ] **2.2 Request Line Parsing**: Extract Method, Path, and HTTP Version from the first line.
- [ ] **2.3 Header Parsing**: Loop through subsequent lines until the double CRLF (`\r\n\r\n`) to extract key-value pairs.
- [ ] **2.4 Validation**: Return a "400 Bad Request" if the input violates the HTTP protocol.

## Phase 3: Routing Engine (The Brain)
*Goal: Determine what to do based on the requested URL.*

- [ ] **3.1 Handler Typedef**: Define a function pointer type: `void (*route_handler)(const http_request_t*, http_response_t*)`.
- [ ] **3.2 Route Registration**: Create a mechanism in `src/router.c` to map strings (paths) to these handler functions.
- [ ] **3.3 URI Matching**: Implement the lookup logic to find the correct handler for an incoming path, defaulting to a 404 handler if not found.

## Phase 4: Response Generation (The Output)
*Goal: Format the structured response back into bytes for the client.*

- [ ] **4.1 String Formatting**: In `src/response.c`, create a function that builds the full HTTP response string (Status Line + Headers + Body).
- [ ] **4.2 Socket Writing**: Implement a reliable `send_all` function to ensure the entire buffer is transmitted over the network.
- [ ] **4.3 Cleanup**: Implement a `free_response` function to prevent memory leaks after the data is sent.

## Phase 5: Integration & Execution
*Goal: Connect all pieces within the server's main loop.*

- [ ] **5.1 Update `server_run`**: Modify the loop in `src/server.c` to:
    1. `accept()` a connection.
    2. Parse the request using `request.c`.
    3. Route the request using `router.c`.
    4. Generate and send the response using `response.c`.
    5. Close the connection.
- [ ] **5.2 Memory Management**: Rigorously check for leaks during the request-response lifecycle.

## Phase 6: Advanced Features
*Goal: Enhance performance and utility.*

- [ ] **6.1 Concurrency**: Integrate `pthreads` to handle each client in a separate thread.
- [ ] **6.2 Static File Serving**: Add a handler that reads and serves files from a `public/` directory.
- [ ] **6.3 Assembly Optimizations**: Implement low-level helpers (I/O, String, Memory) in Assembly as planned in the README.
