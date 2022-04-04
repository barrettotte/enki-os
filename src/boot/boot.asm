; Enki OS Bootloader
;
; TODO: summary
;
        org 0x7C00                          ; BIOS puts us here
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
        jmp $

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
        mov eax, 1                          ; start sector (skip 0=bootloader)
        mov ecx, 100                        ; sectors of kernel   TODO: constant?
        mov edi, 0x0100000                  ; kernel entry        TODO: constant?
        call ata_lba_read                   ; read kernel from hard disk
        jmp CODE_SEG:0x0100000              ; jump into kernel    TODO: constant?

ata_lba_read:                               ; ***** ATA LBA Read *****
        mov ebx, eax                        ; save LBA
        shr eax, 24                         ; highest byte
        or eax, 0xE0                        ; select master drive
        mov dx, 0x1F6                       ; hard disk controller port
        out dx, al                          ; send byte

        mov eax, ecx                        ; total sectors
        mov dx, 0x1F2                       ;
        out dx, al                          ;

        mov eax, ebx                        ; restore LBA
        mov dx, 0x1F3                       ;
        out dx, al                          ;

        mov dx, 0x1F4                       ;
        mov eax, ebx                        ; restore LBA
        shr eax, 8                          ; 
        out dx, al                          ;

        mov dx, 0x1F5                       ;
        mov eax, ebx                        ;
        shr eax, 16                         ; upper 16-bits

        mov dx, 0x1F7                       ;
        mov al, 0x20                        ;
        out dx, al                          ;

.next_sector:                               ;
        push ecx                            ; stash sector_count
.try_read:                                  ;
        mov dx, 0x1F7                       ; 
        in al, dx                           ; 
        test al, 8                          ; check if read was successful
        jz .try_read                        ; repeat read until success

        mov ecx, 256                        ; amount of words to read
        mov dx, 0x1F0                       ; IO port to read from
        rep insw                            ; read 256 words from IO port (DX) starting at ES:EDI

        pop ecx                             ; restore sector_count
        loop .next_sector                   ; while (sector_count < total_sectors)
        ret                                 ; end of ata_lba_read subroutine

.end:                                       ;
        jmp $                               ; hang
                                            ; ***** complete boot sector *****
              times 510 - ($ - $$) db 0x0   ; pad rest of boot sector
              dw 0xAA55                     ; magic numbers; boot record signature