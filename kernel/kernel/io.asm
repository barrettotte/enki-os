        bits 32                             ;
        section .asm                        ;

        global inpb                         ; unsigned char inpb(unsigned short port)
        global inpw                         ; unsigned short inpw(unsigned short port)
        global outb                         ; void outb(unsigned short port, unsigned char val)
        global outw                         ; void outw(unsigned short port, unsigned short val)

inpb:                                       ; ***** input byte from port *****
        push ebp                            ; store base pointer
        mov ebp, esp                        ; set base pointer

        xor eax, eax                        ; clear return value
        mov edx, [ebp + 8]                  ; arg[0] = port
        in al, dx                           ; read from port

        pop ebp                             ; restore base pointer
        ret                                 ; end inpb subroutine

inpw:                                       ; ***** input word from port *****
        push ebp                            ; store base pointer
        mov ebp, esp                        ; set base pointer

        xor eax, eax                        ; clear return value
        mov edx, [ebp + 8]                  ; arg[0] = port
        in ax, dx                           ; read from port

        pop ebp                             ; restore base pointer
        ret                                 ; end inpw subroutine

outb:                                       ; ***** output byte to port *****
        push ebp                            ; store base pointer
        mov ebp, esp                        ; set base pointer

        mov eax, [ebp + 12]                 ; args[1] = byte
        mov edx, [ebp + 8]                  ; args[0] = port
        out dx, al                          ; write to port

        pop ebp                             ; restore base pointer
        ret                                 ; end outb subroutine

outw:                                       ; ***** output word to port *****
        push ebp                            ; store base pointer
        mov ebp, esp                        ; set base pointer

        mov eax, [ebp + 12]                 ; args[1] = word
        mov edx, [ebp + 8]                  ; args[0] = port
        out dx, ax                          ; write to port

        pop ebp                             ; restore base pointer
        ret                                 ; end outw subroutine
