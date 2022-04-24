        bits 32                             ;
        section .asm                        ;

        extern main                         ;
        global _start                       ;
_start:                                     ; ***** entry *****
        call main                           ; call
        ret                                 ;