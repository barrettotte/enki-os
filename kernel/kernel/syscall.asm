        bits 32                             ;
        section .asm                        ;
        
        global syscall_wrapper              ; void syscall_wrapper()

        extern syscall_handler              ;

syscall_wrapper:                            ; ***** Interrupt 0x80 wrapper *****
                                            ; start of interrupt frame
                                            ; processor pushes on entry: uint32_t ip, cs, flags, sp, ss
        pushad                              ; save userland general purpose registers
        push esp                            ; save pointer to interrupt frame (end of frame)
        push eax                            ; kernel command to invoke
        call syscall_handler                ; run kernel command
        mov dword [kernel_result], eax      ; temp save kernel command (C func) result
        
        add esp, 8                          ; restore stack pointer (skip esp and eax pushes)
        popad                               ; restore userland general purpose registers
        mov eax, [kernel_result]            ; return kernel command result

        iretd                               ; end syscall_wrapper subroutine; return from interrupt

        section .data                       ;

kernel_result: dd 0                         ; temporary storage for kernel 0x80 interrupt result
