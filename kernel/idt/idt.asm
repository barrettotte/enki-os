        bits 32                             ;
        section .asm                        ;
        
        global enable_interrupts            ; void enable_interrupts()
        global disable_interrupts           ; void disable_interrupts()
        global idt_load                     ; void idt_load(struct idtr_ptr* ptr)
        global no_interrupt                 ; void no_interrupt()
        global isr_80h_wrapper              ; void isr_80h_wrapper()
        global interrupt_ptr_table          ; void* interrupt_ptr_table[ENKI_TOTAL_INTERRUPTS]

        extern no_interrupt_handler         ;
        extern isr_80h_handler              ;
        extern interrupt_handler            ;

enable_interrupts:                          ; ***** enable interrupts *****
        sti                                 ; enable interrupts
        ret                                 ; end enable_interrupts subroutine

disable_interrupts:                         ; ***** disable interrupts *****
        cli                                 ; clear interrupts
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

; generically create an interrupt
%macro interrupt 1
        global int%1                        ; export interrupt
  int%1:                                    ; ***** interrupt *****
                                            ; start interrupt frame
                                            ;   processor pushes on entry: dword ip, cs, flags, sp, ss
        pushad                              ;   save general purpose registers
                                            ; end interrupt frame
        push esp                            ; save stack pointer
        push dword %1                       ; push interrupt number
        call interrupt_handler              ; handle the interrupt (in C)
        add esp, 8                          ; restore stack pointer (two args = 8 bytes)
        popad                               ; restore general purpose registers
        iret                                ; end of interrupt subroutine; return from interrupt
%endmacro

; create all interrupts
%assign i 0
%rep 512
  interrupt i
  %assign i i+1
%endrep

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

interrupt_ptr_table:                        ; table of pointers to interrupt routines

; add pointer to interrupt routine
%macro int_arr_entry 1
  dd int%1
%endmacro

; add pointers for all interrupts
%assign i 0
  %rep 512
  int_arr_entry i
  %assign i i+1
%endrep
