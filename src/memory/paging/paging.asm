        bits 32                             ;
        section .asm                        ;
        
        global enable_paging                ;
        global paging_load_directory        ;
        
enable_paging:                              ; ***** enable paging *****
        push ebp                            ;
        mov ebp, esp                        ;

        mov eax, cr0                        ; get control register
        or eax, 0x80000000                  ; set PG bit
        mov cr0, eax                        ; set control register

        pop ebp                             ;
        ret                                 ; end enable_paging subroutine
        
paging_load_directory:                      ; ***** load page directory *****
        push ebp                            ;
        mov ebp, esp                        ;

        mov eax, [ebp + 8]                  ;
        mov cr3, eax                        ; address of page directory

        pop ebp                             ;
        ret                                 ; end paging_load_directory subroutine