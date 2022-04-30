        bits 32                             ;
        section .asm                        ;

        global task_return                  ; void task_return(struct registers* regs)
        global restore_gpr                  ; void restore_gpr(struct registers* regs)
        global user_registers               ; void user_registers()

task_return:                                ; ***** change CPU state and return to userland *****
        mov ebp, esp                        ; set base pointer

        mov ebx, [ebp + 4]                  ; pointer to registers struct
        push dword [ebx + 44]               ; data/stack selector
        push dword [ebx + 40]               ; stack pointer
        
        pushf                               ; store flags
        pop eax                             ; load flags
        or eax, 0x200                       ; enable interrupts (after iret)
        push eax                            ; store flags

        push dword [ebx + 32]               ; code segment
        push dword [ebx + 28]               ; instruction pointer (virtual address)

        mov ax, [ebx + 44]                  ; load data/stack selector
        mov ds, ax                          ; data segment
        mov es, ax                          ; extra segment
        mov fs, ax                          ; extra segment
        mov gs, ax                          ; extra segment

        push dword[ebp + 4]                 ; pointer to registers struct
        call restore_gpr                    ; restore general purpose registers
        add esp, 4                          ; restore stack pointer  (don't pop, will corrupt a reg)
                                            
                                            ; leave kernel and exec in userland
        iretd                               ; end task_return subroutine

restore_gpr:                                ; ***** restore general purpose registers *****
                                            ; void restore_gpr(struct registers* regs)
        push ebp                            ; store base pointer
        mov ebp, esp                        ; set base pointer

        mov ebx, [ebp + 8]                  ; restore ebx
        mov edi, [ebx]                      ; restore edi
        mov esi, [ebx + 4]                  ; restore esi
        mov ebp, [ebx + 8]                  ; restore ebp
        mov edx, [ebx + 16]                 ; restore edx
        mov ecx, [ebx + 20]                 ; restore ecx
        mov eax, [ebx + 24]                 ; restore eax
        mov ebx, [ebx + 12]                 ; restore ebx

        add esp, 4                          ; restore stack pointer
        ret                                 ; end restore_gpr subroutine

user_registers:                             ; ***** change seg regs to user data seg regs *****
        mov ax, 0x23                        ; user data segment start
        mov ds, ax                          ; set data segment
        mov es, ax                          ; set extra segment
        mov fs, ax                          ; set extra segment
        mov gs, ax                          ; set extra segment
        ret                                 ; end user_registers subroutine