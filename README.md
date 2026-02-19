### HTTP Server projet layout

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
### Contributing to MyProject

1. Fork the repository
2. Create a new branch: `git checkout -b feature/my-feature`
3. Commit your changes: `git commit -m "Add new feature"`
4. Push to your branch: `git push origin feature/my-feature`
5. Open a Pull Request and wait for review
