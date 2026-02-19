### HTTP Server projet layouty

```
http_server/
├── src/
│   ├── main.c              # Entry point, config, startup
│   ├── server.c            # Socket setup, accept loop
│   ├── request.c           # HTTP request parsing
│   ├── response.c          # HTTP response builder
│   ├── router.c            # Route matching logic
│   └── asm/
│       ├── io.asm          # Low-level read/write syscalls
│       ├── str.asm         # String utilities (scan, copy, compare)
│       └── mem.asm         # Memory helpers (zero, copy buffers)
├── include/
│   ├── server.h
│   ├── request.h
│   ├── response.h
│   └── router.h
├── Makefile
└── README.md
```
