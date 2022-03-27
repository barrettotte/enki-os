; first stage of bootloader

        org 0x0                       ; 
        bits 16                       ; real mode
start:  jmp main                      ; skip over BPB

; *************************************
;       BIOS parameter block
; *************************************

; https://en.wikipedia.org/wiki/BIOS_parameter_block#FAT12_/_FAT16
; https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#EBPB=
;
; BPB needs to be offset 3 bytes from segment start. 
; So, the first far jump above (3 bytes) puts BPB where it needs to be.

bpb_OEM:                 db "Enki OS "     ; Name of OS (8 bytes)

bpb_bytes_per_sector:    dw 512            ; bytes per logical sector
bpb_sectors_per_cluster: db 1              ; logical sectors per cluster
bpb_reserved_sectors:    dw 1              ; number of logical sectors before first FAT in image
bpb_number_of_FAT:       db 2              ; number of file allocation tables
bpb_root_entries:        dw 224            ; max root directory entries
bpb_total_sectors:       dw 2880           ; total logical sectors; 512 * 2880 = 1.44MB
bpb_media_descriptor:    db 0xF0           ; 3.5in double sided, 80 tracks per side, 18 sectors per track
bpb_sectors_per_FAT:     dw 9              ; logical sectors per FAT
bpb_sectors_per_track:   dw 18             ; physical sectors per track (18 = 1.44MB)
bpb_heads_per_cylinder:  dw 2              ; number of read-write heads (2 = double-sided floppy)
bpb_hidden_sectors:      dd 0              ; sectors before partition with FAT volume (0 = unpartitioned)
bpb_total_sectors_big:   dd 0              ; sectors over 32 MB

                                           ; ***** extended BPB *****
eb_drive_number:         db 0             ; physical drive number; 0x0=floppy, 0x80=HDD
eb_unused:               db 0             ; reserved
eb_ext_boot_signature:   db 0x29          ; extended boot signature; MS/PC-DOS version 4.0 
eb_serial_number:        db 0xDEADBEEF    ; volume ID, xxxx-xxxx
eb_volume_label:         db "ENKI FLOPPY" ; partition volume label (11 bytes)
eb_file_system:          db "FAT12   "    ; file system type (8 bytes)

; *************************************
;       Bootloader entry point
; *************************************

main:                                 ; 0x0000:0x7C00
        cli                           ; disable interrupts
        mov ax, 0x07C0                ; set segment registers to our segment
        mov ds, ax                    ;
        mov es, ax                    ;
        mov fs, ax                    ;
        mov gs, ax                    ;

        xor ax, ax                    ; init stack
        mov ss, ax                    ; 
        mov sp, 0xFFFF                ; 
        sti                           ; enable interrupts

        mov si, msg_load              ;
        call print_string             ;


msg_load: db "Loading boot image... ", 0x0D, 0x0A, 0x00

; *************************************
;       Complete boot sector
; *************************************

        times 510 - ($ - $$) db 0     ; pad rest of boot sector
        dw 0xAA55                     ; magic numbers; boot record signature
