; test program - just prints typed characters to terminal via syscall

        bits 32                             ;
        section .asm                        ;

        global _start                       ;

_start:                                     ; ***** entry *****
        push msg                            ; pointer to message
        mov eax, 1                          ; print command
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

input_loop:
        call get_key                        ; get key pressed
        push eax                            ; store response

        mov eax, 3                          ; putchar command
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

        jmp input_loop                      ; infinite loop

get_key:                                    ; ***** get key pressed *****
        mov eax, 2                          ; get_key command
        int 0x80                            ; syscall
        cmp eax, 0x00                       ; check non-zero
        je get_key                          ; while (key == 0)
        ret                                 ; end get_key subroutine

        section .data

msg:    db "Hello from userland"
        db 0x0D, 0x0A, 0x00