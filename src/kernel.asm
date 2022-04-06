; Enki OS kernel
;
; TODO: summary
;
        bits 32                             ; protected mode
        global _start                       ; export symbol
        global test_div0                    ; TODO: remove

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

.remap_pic:                                 ; remap Programmable Interrupt Controller (PIC)
        mov al, 00010001b                   ; init mode
        out 0x20, al                        ; put master PIC into init mode

        mov al, 0x20                        ; master PIC ISR start
        out 0x21, al                        ; 

        mov al, 00000001b                   ; master PIC x86 mode
        out 0x21, al                        ;

.kernel:                                    ;
        call kernel_main                    ; enter kernel
.end:                                       ;
        jmp $                               ; hang

test_div0:                                  ; TODO: this is a test, remove it!
        mov eax, 0
        div eax

        times 512 - ($ - $$) db 0x0         ; pad rest of sector (16-bit align too)