; The PIC routes hardware IRQs to the CPU. This file remaps its vectors
; and handles keyboard/mouse interrupts.

section .data
align 4
global last_scancode, mouse_x, mouse_y, mouse_buttons

mouse_x:       dd 0
mouse_y:       dd 0
last_scancode: dd 0
mouse_buttons: db 0

mouse_packet:       times 3 db 0
mouse_packet_index: db 0

section .text
global keyboard_handler
global mouse_handler
global init_mouse
global pic_remap

PIC1_COMMAND equ 0x20            ; Master PIC command port
PIC1_DATA    equ 0x21            ; Master PIC data/mask port
PIC2_COMMAND equ 0xA0            ; Slave PIC command port
PIC2_DATA    equ 0xA1            ; Slave PIC data/mask port

; Read the keyboard scancode and acknowledge IRQ1.
keyboard_handler:
    pusha
    in al, 0x60                  ; PS/2 controller data port
    movzx eax, al
    mov [last_scancode], eax
    mov al, 0x20                 ; EOI command: tell the PIC the IRQ is handled
    out PIC1_COMMAND, al
    popa
    iret

; Read mouse data and acknowledge IRQ12.
mouse_handler:
    pusha

    ; Read one byte from the mouse.
    in al, 0x60

    movzx ebx, byte [mouse_packet_index]

    ; The first byte has bit 3 set.
    ; Ignore bytes that cannot start a packet.
    cmp bl, 0
    jne .store

    test al, 00001000b
    jz .done

.store:
    mov [mouse_packet + ebx], al
    inc bl

    cmp bl, 3
    jb .save_index

    ; A full packet has arrived.
    mov byte [mouse_packet_index], 0

    ; Store the three button bits.
    mov al, [mouse_packet]
    and al, 00000111b
    mov [mouse_buttons], al

    ; Ignore movement packets with overflow.
    test byte [mouse_packet], 11000000b
    jnz .done

    ; Signed X movement.
    movsx eax, byte [mouse_packet + 1]
    add [mouse_x], eax

    ; Signed Y movement. Screen coordinates usually grow downward.
    movsx eax, byte [mouse_packet + 2]
    sub [mouse_y], eax
    jmp .done

.save_index:
    mov [mouse_packet_index], bl

.done:
    ; Acknowledge both PICs for IRQ12.
    mov al, 0x20
    out PIC2_COMMAND, al
    out PIC1_COMMAND, al

    popa
    iret

init_mouse:
    cli

    ; Enable the PS/2 auxiliary device.
    call ps2_wait_write
    mov al, 0xA8
    out 0x64, al

    ; Read the PS/2 controller configuration byte.
    call ps2_wait_write
    mov al, 0x20
    out 0x64, al
    call ps2_read

    ; Enable IRQ12 and enable the mouse clock.
    or al, 00000010b
    and al, 11011111b
    mov bl, al

    ; Write the updated configuration byte.
    call ps2_wait_write
    mov al, 0x60
    out 0x64, al
    call ps2_wait_write
    mov al, bl
    out 0x60, al

    ; Send the mouse command 0xF4 (enable reporting).
    call ps2_wait_write
    mov al, 0xD4
    out 0x64, al
    call ps2_wait_write
    mov al, 0xF4
    out 0x60, al

    ; Read the mouse acknowledgement (normally 0xFA).
    call ps2_read

    ret

; Wait for a byte, then read it from port 0x60.
ps2_read:
    in al, 0x64
    test al, 1
    jz ps2_read
    in al, 0x60
    ret

ps2_wait_write:
    in al, 0x64
    test al, 00000010b    ; Check if Input Buffer Full flag (bit 1) is set
    jnz ps2_wait_write    ; Wait until buffer is clear before writing
    ret

; The PIC forwards hardware interrupts (IRQs) to the CPU.
; In protected mode, CPU exceptions use vectors 0-31, so we move
; hardware IRQs to vectors 32-47 to avoid conflicts.
;
; Master PIC: IRQ0-7  -> vectors 32-39 (0x20-0x27)
; Slave PIC:  IRQ8-15 -> vectors 40-47 (0x28-0x2F)
;
; The CPU uses the resulting vector number to find the handler in the IDT.
 
pic_remap:
    pusha

    ; ICW1: initialize both PICs
    ; 0x11 = 00010001b:
    ;   bit 4 = 1: this is an initialization command.
    ;   bit 0 = 1: ICW4 WILL follow, so the PIC expects 4 control words.
    ; If bit 0 were 0 (ICW1 = 0x10), ICW4 would be omitted,
    ; and the PIC would expect only ICW2 and ICW3 after ICW1.
    mov al, 0x11
    out PIC1_COMMAND, al
    out PIC2_COMMAND, al
    
    ; ICW2: choose the starting interrupt vector for each PIC.
    mov al, 0x20                 ; Master IRQ0-7 -> vectors 0x20-0x27
    out PIC1_DATA, al
    mov al, 0x28                 ; Slave IRQ8-15 -> vectors 0x28-0x2F
    out PIC2_DATA, al
    
    ; ICW3: describe how the two PICs are connected.
    ; Each PIC handles 8 IRQ inputs, but a PC needs more.
    ; The master handles IRQ0-IRQ7; the slave handles IRQ8-IRQ15.
    ; The slave's output connects to the master's IRQ2 input.
    ; The master forwards slave interrupt requests to the CPU.
    
    ; Master ICW3 = 0x04 = 00000100b:
    ; Each bit represents one master IRQ input.
    ; Bit n = 1 means a slave is connected to IRQn.
    ; Only bit 2 is set, so the slave connects to master IRQ2.
    mov al, 0x04
    out PIC1_DATA, al             ; Master: slave connected to IRQ2

    ; Slave ICW3 = 0x02
    ; The slave uses a number, not a bitmask.
    ; Value 2 identifies the master's IRQ input it connects to.
    ; Both values describe the same connection from opposite sides.
    mov al, 0x02
    out PIC2_DATA, al             ; Slave: connected to master IRQ2
    

    ; ICW4: configure the PIC for 8086-compatible mode.
    ; 0x01 = 00000001b; bit 0 selects 8086 mode (else 8085).
    mov al, 0x01
    out PIC1_DATA, al
    out PIC2_DATA, al
    popa
    ret
