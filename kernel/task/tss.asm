        bits 32                             ;
        section .asm                        ;
        
        global tss_load                     ; void tss_load(int tss_offset)
        
tss_load:                                   ; ***** load task switch segment *****
        push ebp                            ; store base pointer
        mov ebp, esp                        ; set base pointer

        mov ax, [ebp + 8]                   ; arg[0] = tss_offset
        ltr ax                              ; load task register

        pop ebp                             ; restpre base pointer
        ret                                 ; end tss_load subroutine