
; gdb
; target remote | qemu-system-x86_64 -hda boot.bin -S -gdb stdio
; c

; burn to USB
;   sudo fdisk -l                     # find USB disk
;   sudo dd if=boot.bin of=/dev/sdb   # overwrite USB's first sector with our bootloader

        org 0x7C00                          ; 
        bits 16                             ; real mode

CODE_SEG equ gdt_code - gdt_start           ;
DATA_SEG equ gdt_data - gdt_start           ;

_start: jmp short code                      ; jump over disk format info
        nop                                 ;

; NOTE: some BIOS will overwrite the BPB with their own data...?
times 33 db 0                               ; BPB - TODO: do I need data?

code:   jmp 0x0:main                        ; init code segment

main:                                       ; ***** main *****
        cli                                 ; clear interrupts
        mov ax, 0x0                         ;
        mov ds, ax                          ; init data segment
        mov es, ax                          ; init extra segment
        mov ss, ax                          ; init stack segment
        mov sp, 0x7C00                      ; set stack pointer
        sti                                 ; enable interrupts

        ; mov ah, 0x2                         ; read sector fn
        ; mov al, 1                           ; sectors to read
        ; mov ch, 0                           ; cylinder low 8 bits
        ; mov cl, 2                           ; read sector two
        ; mov dh, 0                           ; head number - NOTE: DL set by BIOS (drive No.)
        ; mov bx, buffer                      ; pointer to buffer
        ; int 0x13                            ; BIOS interrupt
        ; jc error                            ; read failed

        mov si, buffer                      ;
        call print                          ;

.load_protected:                            ;
        cli                                 ;
        lgdt [gdt_descriptor]               ; load GDT
        mov eax, cr0                        ;
        or eax, 0x1                         ;
        mov cr0, eax                        ;
        jmp CODE_SEG:load_32                ; 

                                            ; ***** Global Descriptor Table (GDT) *****
gdt_start:                                  ;
gdt_null:                                   ;
        dd 0x0000, 0x0000                   ;
gdt_code:                                   ; code segment (offset 0x8) => CS
        dw 0xFFFF                           ; segment limit first 0-15 bits
        dw 0x00                             ; base first 0-15 bits
        db 0x0                              ; base 16-23 bits
        db 0x9A                             ; access byte
        db 11001111b                        ; flags
        db 0x0                              ; base 24-31 bits
gdt_data:                                   ; data segment (offset 0x10) => DS,SS,ES,FS,GS
        dw 0xFFFF                           ; segment limit first 0-15 bits
        dw 0x00                             ; base first 0-15 bits
        db 0x0                              ; base 16-23 bits
        db 0x92                             ; access byte
        db 11001111b                        ; flags
        db 0x0                              ; base 24-31 bits
gdt_end:                                    ;
gdt_descriptor:                             ;
        dw gdt_end - gdt_start - 1          ; size
        dd gdt_start                        ; offset

print:                                      ; ***** print *****
        mov bx, 0x0                         ;
.more:                                      ;
        lodsb                               ;
        cmp al, 0x0                         ;
        je .done                            ;
        call print_char                     ;
        jmp .more                           ;
.done:                                      ;
        ret                                 ; end print subroutine

print_char:                                 ; ***** print_char *****
        mov ah, 0xE                         ; video - teletype output
        int 0x10                            ; BIOS interrupt
        ret                                 ; end print_char subroutine

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
        or al, 2                            ; enable A20 line
        out 0x92, al                        ; write to port

        jmp $

                                            ; ***** variables *****
err_msg:      db 'Failed to load'           ;
              db ' sector', 0x0             ;
                                            ; ***** complete boot sector *****
              times 510 - ($ - $$) db 0x0   ; pad rest of boot sector
              dw 0xAA55                     ; magic numbers; boot record signature
buffer:
