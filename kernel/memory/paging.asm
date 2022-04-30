        bits 32                             ;
        section .asm                        ;
        
        global enable_paging                ; void enable_paging()
        global paging_load_directory        ; void paging_load_directory(uint32_t* dir)
        
enable_paging:                              ; ***** enable paging *****
        push ebp                            ; store base pointer
        mov ebp, esp                        ; set base pointer

        mov eax, cr0                        ; get control register
        or eax, 0x80000000                  ; set PG bit
        mov cr0, eax                        ; set control register

        pop ebp                             ; restore base pointer
        ret                                 ; end enable_paging subroutine
        
paging_load_directory:                      ; ***** load page directory *****
        push ebp                            ; store base pointer
        mov ebp, esp                        ; set base pointer

        mov eax, [ebp + 8]                  ; arg[0] = pointer to page directory
        mov cr3, eax                        ; address of page directory

        pop ebp                             ; restore base pointer
        ret                                 ; end paging_load_directory subroutine