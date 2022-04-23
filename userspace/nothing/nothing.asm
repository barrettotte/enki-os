        bits 32                             ;
        section .asm                        ;

        global _start                       ;

_start:                                     ; ***** entry *****
        ; push msg
        ; mov eax, 1
        ; int 0x80
        ; add esp, 4

input_loop:
        call get_key
        push eax

        mov eax, 3
        int 0x80
        add esp, 4
        jmp input_loop

        jmp $                               ; infinite loop

get_key:                                    ; ***** get key pressed *****
        mov eax, 2                          ; get_key command
        int 0x80                            ; syscall
        cmp eax, 0x00                       ; check non-zero
        je get_key                          ; while (key == 0)
        ret                                 ; end get_key subroutine

        section .data

msg:    db "Hello from userland"
        db 0x0D, 0x0A, 0x00