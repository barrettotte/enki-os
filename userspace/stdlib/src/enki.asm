        bits 32                             ;
        section .asm                        ;

        global print:function               ; void print(const char* msg)
        global enki_getkey:function         ; int enki_getkey()
        global enki_putchar:function        ; void enki_putchar(char c)
        global enki_malloc:function         ; void* enki_malloc(size_t size)
        global enki_free:function           ; void enki_free(void* ptr)

print:                                      ; ***** print message to terminal *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        push dword[ebp + 8]                 ; arg[0] (msg)
        mov eax, 1                          ; print command
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end print subroutine

enki_getkey:                                ; ***** get key pressed *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 2                          ; get_key command  (returned in eax)
        int 0x80                            ; syscall

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_getkey subroutine

enki_putchar:                               ; ***** print character to terminal *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 3                          ; putchar command
        push dword[ebp + 8]                 ; arg[0] (c)
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_putchar subroutine

enki_malloc:                                ; ***** allocate heap memory *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 4                          ; malloc command
        push dword[ebp + 8]                 ; arg[0] (size)
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_malloc subroutine

enki_free:                                  ; ***** free heap memory *****
        push ebp                            ; save base pointer
        mov ebp, esp                        ; set base pointer to stack pointer

        mov eax, 5                          ; free command
        push dword[ebp + 8]                 ; arg[0] (ptr)
        int 0x80                            ; syscall
        add esp, 4                          ; restore stack pointer

        pop ebp                             ; restore base pointer
        ret                                 ; end enki_free subroutine