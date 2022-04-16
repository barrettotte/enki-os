        bits 32                             ; protected mode
        section .asm                        ;
        
        global tss_load                     ;
        
tss_load:                                   ; ***** load task switch segment *****
        push ebp                            ;
        mov ebp, esp                        ;

        mov ax, [ebp + 8]                   ; TSS
        ltr ax                              ; load task register

        pop ebp                             ;
        ret                                 ; end tss_load subroutine