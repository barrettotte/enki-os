        bits 32                             ;
        section .asm                        ;

        global _start                       ;

_start:                                     ; ***** entry *****
        push msg                            ; pointer to message
        mov eax, 0x1                        ; print command
        int 0x80
        add esp, 4                          ; restore stack (1 arg)

        jmp $                               ; infinite loop

        section .data                       ;
msg:    db "Hello from userland", 0x00      ;