; Enki OS kernel
;
; TODO: summary
;
        bits 32                             ; protected mode
        global _start                       ; export symbol
        extern kernel_main                  ;

        CODE_SEG equ 0x08                   ; kernel code segment
        DATA_SEG equ 0x10                   ; kernel data segment
_start:                                     ;
        mov ax, DATA_SEG                    ;
        mov ds, ax                          ; init data segment
        mov es, ax                          ; init extra segment
        mov fs, ax                          ; init extra segment
        mov gs, ax                          ; init extra segment
        mov ss, ax                          ; init stack segment
        mov ebp, 0x00200000                 ; init base pointer
        mov esp, ebp                        ; init stack pointer
.a20:                                       ; enable A20 line
        in al, 0x92                         ; read from port
        or al, 2                            ; fast A20; NOTE: could fail depending on system...
        out 0x92, al                        ; write to port
.kernel:                                    ;
        call kernel_main                    ; enter kernel
.end:                                       ;
        jmp $                               ; hang

        times 512 - ($ - $$) db 0x0         ; pad rest of sector