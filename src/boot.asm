[org 0x7c00]

; Infinite loop
start:
        jmp $

; Bootloader = 512 bytes
; Signature i.e. AA55 = 2 bytes
; Pad remaining bytes with 0
times 510-($-$$) db 0
dw 0xaa55
