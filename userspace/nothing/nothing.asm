        bits 32                             ;
        section .asm                        ;

        global _start                       ;

_start:                                     ; ***** entry *****
        push 20                             ; arg 2
        push 30                             ; arg 1
        mov eax, 0x0                        ; syscall number (sum)
        int 0x80                            ; invoke syscall
        add esp, 8                          ; restore stack pointer (2 args * 4)

        jmp $                               ; infinite loop