section .text

global asm_read
global_asm write

asm_read:
    ; args rdi = fd, rsi = buf, rdx = count
    ; syscall number for read is 0
    mov rax, 0
    syscall

asm_write:
    ; args: rdi = fd, rsi = buf, rdx = count
    ; syscall number for write = 1
    mov rax, 1
    syscall