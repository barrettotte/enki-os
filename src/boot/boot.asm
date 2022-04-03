; Bootloader
;
; TODO: summary
;
        org 0x7C00                          ; 
        bits 16                             ; real mode

        CODE_SEG equ gdt_code - gdt_start   ; protected mode code segment
        DATA_SEG equ gdt_data - gdt_start   ; protected mode data segment

_start: jmp short code                      ; jump over disk format info
        nop                                 ;

times 33 db 0                               ; BPB - TODO:

code:   jmp 0x0:main                        ; init code segment

main:                                       ; ***** main *****
        cli                                 ; clear interrupts
        xor ax, ax                          ;
        mov ds, ax                          ; init data segment
        mov es, ax                          ; init extra segment
        mov ss, ax                          ; init stack segment
        mov sp, 0x7C00                      ; set stack pointer
        sti                                 ; enable interrupts

.load_protected:                            ; setup protected mode
        cli                                 ; clear interupts
        mov si, msg_pmode                   ; pointer to debug message
        call print_str                      ; print debug message

        lgdt [gdt_descriptor]               ; load GDT
        mov eax, cr0                        ;
        or eax, 0x1                         ;
        mov cr0, eax                        ;
        jmp CODE_SEG:load_32                ; enter protected mode

print_str:                                  ; ***** print string to terminal *****
                                            ; in SI - pointer to string
                                            ; clobbers BX
        xor bx, bx                          ; reset terminal background/page
.more:                                      ; loop over string
        lodsb                               ; load next byte of string
        cmp al, 0x0                         ; check for null terminator
        je .done                            ; 
        call print_char                     ; print char to terminal
        jmp .more                           ; while (AL != '\0')
.done:                                      ;
        ret                                 ; end print_str subroutine

print_char:                                 ; ***** print_char *****
                                            ; in AL - char to print
                                            ; in BX - terminal background/page
                                            ; clobbers AH
        mov ah, 0xE                         ; video - teletype output
        int 0x10                            ; BIOS interrupt
        ret                                 ; end print_char subroutine

                                            ; ***** variables *****
msg_pmode:    db 'Protected mode...',       ; debug
              db 0x0D, 0x0A, 0x00           ;
                                            ; ***** Global Descriptor Table (GDT) *****
gdt_start:                                  ;
gdt_null:                                   ;
              dd 0x0000, 0x0000             ;
gdt_code:                                   ; code segment (offset 0x8) => CS
              dw 0xFFFF                     ; [0-15]  limit
              dw 0x00                       ; [16-31] base 16-bit
              db 0x0                        ; [32-39] base 8-bit
              db 10011010b                  ; [40-47] access byte
              db 11001111b                  ; [48-51] limit
                                            ; [52-55] flags
              db 0x0                        ; [56-63] base
gdt_data:                                   ; data segment (offset 0x10) => DS,SS,ES,FS,GS
              dw 0xFFFF                     ; [0-15]  limit
              dw 0x00                       ; [16-31] base 16-bit
              db 0x0                        ; [32-39] base 8-bit
              db 10010010b                  ; [40-47] access byte
              db 11001111b                  ; [48-51] limit
                                            ; [52-55] flags
              db 0x0                        ; [56-63] base
gdt_end:                                    ;
gdt_descriptor:                             ;
              dw gdt_end - gdt_start - 1    ; size
              dd gdt_start                  ; offset

        bits 32                             ; ***** protected mode *****
load_32:                                    ;
        mov ax, DATA_SEG                    ;
        mov ds, ax                          ; init data segment
        mov es, ax                          ; init extra segment
        mov fs, ax                          ; init extra segment
        mov gs, ax                          ; init extra segment
        mov ss, ax                          ; init stack segment
        mov ebp, 0x00200000                 ; init base pointer
        mov esp, ebp                        ; init stack pointer

        in al, 0x92                         ; read from port
        or al, 2                            ; enable A20 line - NOTE: could fail depending on system...
        out 0x92, al                        ; write to port
.end:                                       ;
        jmp $                               ; hang
                                            ; ***** complete boot sector *****
              times 510 - ($ - $$) db 0x0   ; pad rest of boot sector
              dw 0xAA55                     ; magic numbers; boot record signature