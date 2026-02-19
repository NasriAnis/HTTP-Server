section .text

global asm_memset
global asm_memcpy
global asm_bzero

asm_memset:
    ; rdi = buffer, rsi = byte value, rdx = count
    ; TODO: fill rdx bytes of rdi with sil
    ret

asm_memcpy:
    ; rdi = dst, rsi = src, rdx = count
    ; TODO: copy rdx bytes from rsi to rdi
    ret

asm_bzero:
    ; rdi = buffer, rsi = count
    xor al, al               ; al = 0
.loop:
    test rsi, rsi            ; check if count is 0
    je   .done
    mov  byte [rdi], al      ; write 0 to current byte
    inc  rdi                 ; move forward
    dec  rsi                 ; decrement count
    jmp  .loop
.done:
    ret