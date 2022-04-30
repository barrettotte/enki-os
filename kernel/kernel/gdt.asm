        bits 32                             ;
        section .asm                        ;
        
        global gdt_load                     ; void gdt_load(struct gdt* gdt, int size)
        
gdt_load:                                   ; ***** load GDT *****
        mov eax, [esp + 4]                  ; get pointer to GDT
        mov [gdt_descriptor + 2], eax       ; store GDT pointer
        mov ax, [esp + 8]                   ; get size of GDT
        mov [gdt_descriptor], ax            ; store size
        lgdt [gdt_descriptor]               ; load GDT
        ret                                 ; end of gdt_load subroutine

        section .data                       ;

gdt_descriptor:                             ;
        dw 0x00                             ; size
        dd 0x00                             ; start address
