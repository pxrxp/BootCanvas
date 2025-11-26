[org 0x7c00]

extern seg_stack_setup_msg
extern print_string

start:
        xor ax, ax            ; ax = 0
        mov ds, ax            ; Clear segment registers using ax
        mov es, ax
        mov ss, ax
        mov sp, 0x8888        ; Set Stack Pointer to allow calling functions
                              ; Because, function call stores return address on the stack
                              ; 0x8888 - 0x7c00 - 512 = 2696 bytes for stack, arbitrarily chosen.
                               
        mov si, seg_stack_setup_msg
        call print_string

        jmp $                 ; Infinite loop


; Bootloader = 512 bytes, Signature i.e. AA55 = 2 bytes
; Pad remaining bytes with 0
times 510-($-$$) db 0
dw 0xaa55
