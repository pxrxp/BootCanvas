; The PIC routes hardware IRQs to the CPU. This file remaps its vectors
; and handles keyboard/mouse interrupts.

section .data
align 4
global last_scancode, mouse_x, mouse_y

mouse_x:       dd 0
mouse_y:       dd 0
last_scancode: dd 0

section .text
global keyboard_handler
global mouse_handler
global pic_remap

PIC1_COMMAND equ 0x20            ; Master PIC command port
PIC1_DATA    equ 0x21            ; Master PIC data/mask port
PIC2_COMMAND equ 0xA0            ; Slave PIC command port
PIC2_DATA    equ 0xA1            ; Slave PIC data/mask port

; Read the keyboard scancode and acknowledge IRQ1.
keyboard_handler:
    pusha
    in al, 0x60                  ; PS/2 controller data port
    mov ebx, last_scancode
    mov [ebx], al
    mov al, 0x20                 ; EOI command: tell the PIC the IRQ is handled
    out PIC1_COMMAND, al
    popa
    iret

; Read mouse data and acknowledge IRQ12.
; Currently counts interrupts instead of decoding mouse movement.
mouse_handler:
    pusha
    in al, 0x60                  ; PS/2 controller data port
    mov ebx, mouse_x
    inc dword [ebx]
    mov al, 0x20                 ; EOI command
    out PIC2_COMMAND, al         ; Acknowledge slave PIC first
    out PIC1_COMMAND, al         ; Then acknowledge master PIC
    popa
    iret


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
