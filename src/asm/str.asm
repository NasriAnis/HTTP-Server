section .text

global asm_strlen
global asm_strchr
global asm_strncmp
global asm_strncpy

asm_strlen:
    ; rdi = string pointer
    xor rax, rax          ; rax = 0, will be our counter
.loop:
    cmp byte [rdi + rax], 0   ; check if current byte is null terminator
    je  .done                  ; if yes, we're done
    inc rax                    ; otherwise increment counter
    jmp .loop                  ; loop again
.done:
    ret                        ; return length in rax

asm_strchr:
    ; rdi = string pointer, rsi = character to find
    xor rax, rax
.loop:
    mov al, byte [rdi]        ; load current byte
    cmp al, sil               ; compare with target character
    je  .found                ; if match, return pointer
    cmp al, 0                 ; check for null terminator
    je  .notfound             ; if end of string, return NULL
    inc rdi                   ; move to next byte
    jmp .loop
.found:
    mov rax, rdi              ; return pointer to matching character
    ret
.notfound:
    xor rax, rax              ; return NULL
    ret

asm_strncmp:
    ; rdi = string a, rsi = string b, rdx = max bytes
    ; TODO: compare byte by byte up to rdx, return difference in rax
    ret

asm_strncpy:
    ; rdi = dst, rsi = src, rdx = max bytes
    ; TODO: copy bytes from src to dst up to rdx
    ret