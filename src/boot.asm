[org 0x7c00]

start:
        xor ax, ax            ; ax = 0
        mov ds, ax            ; Clear segment registers using ax
        mov es, ax
        mov ss, ax
        mov sp, 0x8888        ; Set Stack Pointer to allow calling functions
                              ; 0x8888-0x7c00-512 = 2696 bytes for stack, arbitrarily chosen.
        
        mov si, msg_hello
        call print_string
        
        jmp $                 ; Infinite loop

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

; 13 = \r
; 10 = \n
;  0 = \0
msg_hello db 'Hello World', 13, 10, 0

; Bootloader = 512 bytes
; Signature i.e. AA55 = 2 bytes
; Pad remaining bytes with 0
times 510-($-$$) db 0
dw 0xaa55
