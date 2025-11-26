extern print_string

print_string:
        mov ah, 0x0e
.repeat:
        lodsb                 ; 1 byte [si]->al
        cmp al, 0             ; al == \0 ?
        je .done              ; Yes => End Loop
        int 0x10              ; No  => BIOS interrupt
        jmp .repeat           ;        Loop back
.done:
        ret
