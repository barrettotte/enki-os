; Enki OS Bootloader

        org 0x7C00                          ; BIOS puts us here
        bits 16                             ; real mode

        CODE_SEG equ gdt_code - gdt_start   ; protected mode code segment
        DATA_SEG equ gdt_data - gdt_start   ; protected mode data segment

_start: jmp short code                      ; short jump over disk format info
        nop                                 ;
                                            ; ***** BIOS Parameter Block *****
bpb_OEM:                 db "ENKI OS "      ; Name of OS (8 bytes)
bpb_bytes_per_sector:    dw 0x200           ; bytes per logical sector
bpb_sect_per_cluster:    db 0x80            ; logical sectors per cluster
bpb_reserved_sectors:    dw 200             ; number of logical sectors before FAT (big kernel = big reserved sectors)
bpb_number_of_FAT:       db 0x02            ; number of file allocation tables (usually 2)
bpb_root_entries:        dw 0x40            ; max root directory entries 
bpb_total_sectors:       dw 0x00            ; total logical sectors; if 0, more than 65535 sectors in volume
bpb_media_descriptor:    db 0xF8            ; hard disk
bpb_sectors_per_FAT:     dw 0x100           ; logical sectors per FAT
bpb_sectors_per_track:   dw 0x20            ; physical sectors per track
bpb_heads_per_cylinder:  dw 0x40            ; number of read-write heads
bpb_hidden_sectors:      dd 0x00            ; sectors before partition with FAT volume (0 = unpartitioned)
bpb_total_sectors_big:   dd 0x773594        ; large sector count (set if bpb_total_sectors = 0)

                                            ; ***** extended BPB *****
eb_drive_number:         db 0x80            ; physical drive number; 0x0=floppy, 0x80=HDD
eb_win_nt:               db 0x00            ; reserved (or WinNT flags)
eb_ext_boot_signature:   db 0x29            ; extended boot signature; MS/PC-DOS version 4.0 
eb_serial_number:        dd 0xDEADBEEF      ; volume ID, xxxx-xxxx
eb_volume_label:         db "ENKI BOOT  "   ; partition volume label (11 bytes)
eb_file_system:          db "FAT16   "      ; file system type (8 bytes)

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
        shr eax, 24                         ; bits 24-27 of LBA
        or eax, 0xE0                        ; select master drive
        mov dx, 0x1F6                       ; set port
        out dx, al                          ; send byte

        mov eax, ecx                        ; total sectors
        mov dx, 0x1F2                       ; set port
        out dx, al                          ; send to port

        mov eax, ebx                        ; bits 0-7 of LBA
        mov dx, 0x1F3                       ; set port
        out dx, al                          ; send to port

        mov eax, ebx                        ; restore LBA
        mov dx, 0x1F4                       ; set port
        shr eax, 8                          ; bits 8-15 of LBA
        out dx, al                          ; send to port

        mov eax, ebx                        ; restore LBA
        mov dx, 0x1F5                       ; set port
        shr eax, 16                         ; bits 16-12 of LBA
        out dx, al                          ; send to port

        mov dx, 0x1F7                       ; set port
        mov al, 0x20                        ; command = read sector(s) with retry
        out dx, al                          ; send to port

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

                                            ; ***** complete boot sector *****
              times 510 - ($ - $$) db 0x0   ; pad rest of boot sector
              dw 0xAA55                     ; magic numbers; boot record signature