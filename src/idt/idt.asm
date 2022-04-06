        section .asm
        global idt_load                     ; export symbol

idt_load:                                   ; ***** Load IDT *****
        push ebp                            ; store frame base pointer of calling function
        mov ebp, esp                        ; set function's own frame pointer

        mov ebx, [ebp + 8]                  ; get first arg (4 * (n + 2)), the pointer to IDT
        lidt [ebx]                          ; load IDT with pointer

        pop ebp                             ; restore calling function frame
        ret                                 ; end idt_load subroutine
