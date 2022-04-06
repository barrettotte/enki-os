        section .asm
        
        global enable_interrupts            ;
        global disable_interrupts           ;
        global idt_load                     ;
        global no_interrupt                 ;
        global int_21h                      ;

        extern int_21h_handler              ;
        extern no_interrupt_handler         ;

enable_interrupts:                          ; ***** enable interrupts *****
        sti                                 ;
        ret                                 ; end enable_interrupts subroutine

disable_interrupts:                         ; ***** disable interrupts *****
        cli                                 ;
        ret                                 ; end disable_interrupts subroutine

idt_load:                                   ; ***** Load IDT *****
                                            ; TODO: args, return, signature
        push ebp                            ; store frame base pointer of calling function
        mov ebp, esp                        ; set function's own frame pointer

        mov ebx, [ebp + 8]                  ; get first arg (4 * (n + 2)), the pointer to IDT
        lidt [ebx]                          ; load IDT with pointer

        pop ebp                             ; restore calling function frame
        ret                                 ; end idt_load subroutine

no_interrupt:                               ; ***** no interrupt *****
        cli                                 ; clear interrupts
        pushad                              ; store all GPRs
        call no_interrupt_handler           ;
        popad                               ; restore GPRs
        sti                                 ; restore interrupts
        iret                                ; return from interrupt; end no_interrupt subroutine

int_21h:                                    ; ***** TODO: *****
        cli                                 ; clear interrupts
        pushad                              ; store all GPRs
        call int_21h_handler                ;
        popad                               ; restore GPRs
        sti                                 ; restore interrupts
        iret                                ; return from interrupt; end int_21h subroutine