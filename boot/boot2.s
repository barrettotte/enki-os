; second stage of bootloader

        org 0x0                       ; 0x1000:0x0
        bits 16                       ; real mode
        jmp main                      ;

main:
        cli                           ; clear interrupts
        push cs                       ;
        pop ds                        ; DS=CS 

        mov si, msg                   ; pointer to message
        call print_string             ; print string

        cli                           ; clear interrupts
        hlt                           ; halt system


print_string:                         ; print string DS=>SI
        lodsb
        or al, al
        jz _print_done
        mov ah, 0x0E
        int 0x10
        jmp print_string
_print_done:
        ret

msg:    db "Welcome to Enki OS!", 0x0A, 0x0D, 0
