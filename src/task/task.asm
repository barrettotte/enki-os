        bits 32                             ; protected mode
        section .asm                        ;

        global task_return                  ;
        global restore_gpr                  ;
        global user_registers               ;
        
task_return:                                ; ***** drop into userland *****
        mov ebp, esp                        ;

        mov ebx, [ebp + 4]                  ; pointer to registers struct
        push dword [ebx + 44]               ; data/stack selector
        push dword [ebx + 40]               ; stack pointer
        
        pushf                               ;
        pop eax                             ;
        or eax, 0x200                       ; enable interrupts (after iret)
        push eax                            ;

        push dword [ebx + 32]               ; code segment
        push dword [ebx + 28]               ; instruction pointer (virtual address)

        mov ax, [ebx + 44]                  ; load data/stack selector
        mov ds, ax                          ; data segment
        mov es, ax                          ; extra segment
        mov fs, ax                          ; extra segment
        mov gs, ax                          ; extra segment

        push dword[ebp + 4]                 ; pointer to registers struct
        call restore_gpr                    ; restore general purpose registers
        add esp, 4                          ; can't pop, but need to get around prior push

        iretd                               ; end task_return subroutine (enter userland)

restore_gpr:                                ; ***** restore general purpose registers *****
                                            ; void restore_gpr(struct registers* regs)
        push ebp                            ;
        mov ebp, esp                        ;

        mov ebx, [ebp + 8]                  ;
        mov edi, [ebx]                      ;
        mov esi, [ebx + 4]                  ;
        mov ebp, [ebx + 8]                  ;
        mov edx, [ebx + 16]                 ;
        mov ecx, [ebx + 20]                 ;
        mov eax, [ebx + 24]                 ;
        mov ebx, [ebx + 12]                 ;

        pop ebp                             ;
        ret                                 ; end restore_gpr subroutine

user_registers:                             ; ***** change seg regs to user data seg regs *****
        mov ax, 0x23                        ; user data segment start
        mov ds, ax                          ; set data segment
        mov es, ax                          ; set extra segment
        mov fs, ax                          ; set extra segment
        mov gs, ax                          ; set extra segment
        ret                                 ; end user_registers subroutine