        bits 32                             ;
        section .asm                        ;

        extern enki_start                   ; void enki_start()
        extern enki_exit                    ; void enki_exit()

        global _start                       ;

_start:                                     ; ***** entry *****
        call enki_start                     ; call C starter (access to argc/argv)
        call enki_exit                      ; exit current process
        ret                                 ; end _start subroutine