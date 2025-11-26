[org 0x7c00]

; Infinite loop
start:
        mov si, msg_hello
        call print_string
        
        jmp $

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
