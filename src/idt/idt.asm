        bits 32                             ;
        section .asm                        ;
        
        global enable_interrupts            ;
        global disable_interrupts           ;
        global idt_load                     ;
        global no_interrupt                 ;
        global int_21h                      ;
        global isr_80h_wrapper              ;

        extern int_21h_handler              ;
        extern no_interrupt_handler         ;
        extern isr_80h_handler              ;

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
        pushad                              ; store all GPRs
        call no_interrupt_handler           ;
        popad                               ; restore GPRs
        iret                                ; end no_interrupt subroutine; return from interrupt

int_21h:                                    ; ***** TODO: *****
        pushad                              ; store all GPRs
        call int_21h_handler                ;
        popad                               ; restore GPRs
        iret                                ; return from interrupt; end int_21h subroutine

isr_80h_wrapper:                            ; ***** Interrupt 0x80 wrapper *****
                                            ; start of interrupt frame
                                            ; processor pushes on entry: uint32_t ip, cs, flags, sp, ss
        pushad                              ; save userland general purpose registers
        push esp                            ; save pointer to interrupt frame (end of frame)
        push eax                            ; kernel command to invoke
        call isr_80h_handler                ; run kernel command
        mov dword [kernel_result], eax      ; temp save kernel command (C func) result
        
        add esp, 8                          ; restore stack pointer (skip esp and eax pushes)
        popad                               ; restore userland general purpose registers
        mov eax, [kernel_result]            ; return kernel command result

        iretd                               ; end isr_80h_wrapper subroutine; return from interrupt

        section .data                       ;

kernel_result: dd 0                         ; temporary storage for kernel 0x80 interrupt result