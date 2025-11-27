[org 0x7c00]

start:
        ; BIOS only loads sector 1 to 0x7c00
        ; We'll use other sectors to store the kernel.
        ; So we'll later need to use BIOS interrupts to do so.
         
        mov [boot_drive], dl      ; BIOS passes boot drive number in dl

        xor ax, ax                ; ax = 0
        mov ds, ax                ; Clear data segment register using ax
        mov es, ax                ; Clear extra segment register using ax
        mov ss, ax                ; Clear stack segment register using ax
        mov sp, 0x8000            ; Set stack pointer (needed for function calls)
                                  ; When function called, return address stored on stack

        mov si, msg_init
        call print_string

        mov si, dap
        mov dl, [boot_drive]
        mov ah, 0x42
        int 0x13
        jc disk_error
        mov si, msg_load_success
        call print_string
        jmp 0x0000:0x1000         ; Jump to memory sector 2 successfully loaded into

        jmp $                     ; Infinite loop


;================================================================================================
 

disk_error:
       mov si, msg_error
        call print_string
        jmp $


print_string:
        mov ah, 0x0e
.repeat:
        lodsb                     ; 1 byte [si]->al
        cmp al, 0                 ; al == \0 ?
        je .done                  ; Yes => End Loop
        int 0x10                  ; No  => BIOS interrupt
        jmp .repeat               ;        Loop back
.done:
        ret


;================================================================================================

; 13=\r, 10=\n, 0=\0 (null terminated strings)
boot_drive db 0x00
msg_init db 'Cleared segment registers and setup stack.', 13, 10, 0
msg_load_start db 'Attempting to load Sector 2...', 13, 10, 0
msg_load_success db 'Load successful. Jumping to kernel...', 13, 10, 0
msg_error db 'DISK READ ERROR!', 13, 10, 0


align 16  
dap:                              ; disk address packet (given to bios)
        db 16                     ; size of packet
        db 0                      ; reserved
        dw __NUM_SECTORS__        ; number of sectors to read (will be replaced by sed)
        dw 0x1000                 ; offset (0x1000)
        dw 0                      ; segment (0x0000) => Destination of read = 0x0000:0x1000 
        dq 1                      ; LBA start (1 = first sector after boot sector)

;================================================================================================


; Bootloader = 512 bytes, Signature i.e. AA55 = 2 bytes
; Pad remaining bytes with 0
times 510-($-$$) db 0
dw 0xaa55
