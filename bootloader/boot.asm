      [org 0x7C00]             ; set boot sector offset
_start:
      mov bp, 0x8000           ; init base stack pointer
      mov sp, bp               ; init top stack pointer to base

      mov ah, 0x0E             ; set TTY mode
      mov al, [boot_msg]       ; boot_msg[0]
      int 0x10                 ; BIOS video service

; TODO: define GDT
; TODO: switch to protected mode

_done:
      jmp $                    ; hang

boot_msg:
      db 'X'                   ; test char

      times 510-($-$$) db 0    ; pad boot sector
      dw 0xAA55                ; boot sector signature

; TODO: makefile?
; nasm -f bin boot.asm -o boot.bin
; qemu-system-x86_64 boot.bin
