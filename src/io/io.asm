        section .asm

        global inpb                         ;
        global inpw                         ;
        global outb                         ;
        global outw                         ;

inpb:                                       ; ***** input byte from port *****
                                            ; TODO: args, return, signature
        push ebp                            ; store frame base pointer of calling function
        mov ebp, esp

        xor eax, eax                        ; clear return value
        mov edx, [ebp + 8]                  ; arg[0] = port
        in al, dx                           ;

        pop ebp
        ret                                 ; end inpb subroutine

inpw:                                       ; ***** input word from port *****
                                            ; TODO: args, return, signature
        push ebp                            ;
        mov ebp, esp                        ;

        xor eax, eax                        ;
        mov edx, [ebp + 8]                  ;
        in ax, dx                           ;

        pop ebp                             ;
        ret                                 ; end inpw subroutine

outb:                                       ; ***** output byte to port *****
                                            ; TODO: args, return, signature
        push ebp                            ;
        mov ebp, esp                        ;

        mov eax, [ebp + 12]                 ; args[1] = byte
        mov edx, [ebp + 8]                  ; args[0] = port
        out dx, al                          ;

        pop ebp                             ;
        ret                                 ; end outb subroutine

outw:                                       ; ***** output word to port *****
                                            ; TODO: args, return, signature
        push ebp                            ;
        mov ebp, esp                        ;

        mov eax, [ebp + 12]                 ; args[1] = word
        mov edx, [ebp + 8]                  ; args[0] = port
        out dx, ax                          ;

        pop ebp                             ;
        ret                                 ; end outw subroutine
