        bits 32                             ;
        section .asm                        ;

        ;                                   ; 0:
        global enki_print:function          ; 1: void enki_print(const char* msg)
        global enki_getkey:function         ; 2: int enki_getkey()
        global enki_putchar:function        ; 3: void enki_putchar(char c)
        global enki_malloc:function         ; 4: void* enki_malloc(size_t size)
        global enki_free:function           ; 5: void enki_free(void* ptr)
        global enki_process_start:function  ; 6: void enki_process_start(const char* file_name)
        global enki_system:function         ; 7: int enki_system(struct cmd_arg* args)
        global enki_process_args:function   ; 8: void enki_process_args(struct process_args* args)
        global enki_exit:function           ; 9: void enki_exit()

enki_print:                                 ; ***** print message to terminal *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        push dword[ebp + 8]                 ; arg[0] (msg)
        mov eax, 1                          ; command 1 - print
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_print subroutine

enki_getkey:                                ; ***** get key pressed *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 2                          ; command 2 - get_key
        int 0x80                            ; syscall  (key returned in eax)

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_getkey subroutine

enki_putchar:                               ; ***** print character to terminal *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 3                          ; command 3 - putchar
        push dword[ebp + 8]                 ; arg[0] (c)
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_putchar subroutine

enki_malloc:                                ; ***** allocate heap memory *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 4                          ; command 4 - malloc
        push dword[ebp + 8]                 ; arg[0] (size)
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_malloc subroutine

enki_free:                                  ; ***** free heap memory *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 5                          ; command 5 - free
        push dword[ebp + 8]                 ; arg[0] (ptr)
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_free subroutine

enki_process_start:                         ; ***** start a process *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 6                          ; command 6 - process_start
        push dword[ebp + 8]                 ; arg[0] (file_name)
        int 0x80                            ; syscall - important: will not return from this interrupt
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_process_start subroutine

enki_system:                                ; ***** system command *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 7                          ; command 7 - system
        push dword[ebp + 8]                 ; arg[0] (args)
        int 0x80                            ; syscall - important: will not return from this interrupt
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_system subroutine

enki_process_args:                          ; ***** get arguments for a process *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 8                          ; command 8 - process_args
        push dword[ebp + 8]                 ; arg[0] (args)
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_process_args subroutine

enki_exit:                                  ; ***** exit current process *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 9                          ; command 9 - exit
        int 0x80                            ; syscall

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_exit subroutine