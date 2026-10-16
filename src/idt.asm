; The IDT tells the CPU which handler to run for each interrupt.
; Each 8-byte entry stores the handler address, GDT code selector, and flags.

section .data
align 4
global idtr

idtr:
    dw idt_end - idt_start - 1   ; IDT size in bytes minus 1
    dd idt_start                 ; Address of the IDT

section .bss
align 16
global idt_start
idt_start:
    resb 256 * 8                 ; 256 interrupt vectors, 8 bytes each
idt_end:

section .text
global set_idt_gate
global setup_interrupts
extern keyboard_handler
extern mouse_handler
extern remap_pic
extern init_mouse
extern GDT_CODE_SEL

PIC1_COMMAND equ 0x20            ; Master PIC command port
PIC1_DATA    equ 0x21            ; Master PIC data/mask port
PIC2_COMMAND equ 0xA0            ; Slave PIC command port
PIC2_DATA    equ 0xA1            ; Slave PIC data/mask port

; EAX = handler address, DL = interrupt vector number.
set_idt_gate:
    push ebx
    mov ebx, eax
    movzx eax, dl
    shl eax, 3                   ; Each entry is 8 bytes: vector * 8
    add eax, idt_start           ; Address of the selected IDT entry

    mov word [eax], bx                  ; Lower 16 bits of handler address
    mov word [eax + 2], GDT_CODE_SEL    ; Code segment selector from the GDT
    mov byte [eax + 4], 0               ; Reserved byte
    mov byte [eax + 5], 10001110b       ; Present, ring 0, 32-bit interrupt gate (1, 00, 01110)
                                        ; If Present is not set to 1, an exception will occur
                                        ; DPL: Descriptor Privilege Level
                                        ; 00 = ring0, 01 = ring1, 10 = ring2, 11 = ring3
    shr ebx, 16                         ; Move upper 16 address bits into the lower half
    mov word [eax + 6], bx              ; Upper 16 bits of handler address
    pop ebx
    ret

setup_interrupts:
    call remap_pic

    mov eax, keyboard_handler
    mov edx, 0x21                ; IRQ1 (keyboard) -> vector 0x21
    call set_idt_gate

    mov eax, mouse_handler
    mov edx, 0x2C                ; IRQ12 (mouse) -> vector 0x2C
    call set_idt_gate

    lidt [idtr]                  ; Load IDT address and limit into the CPU

    call init_mouse

    mov al, 0xF9                 ; 11111001: 0=allow, 1=block.
    out PIC1_DATA, al            ; Bits 1-2 are 0: allow IRQ1, IRQ2.
                                 ; IRQ2 connects the slave PIC to the master.
    mov al, 0xEF                 ; 11101111: bit 4 is 0, allowing slave IRQ12.
    out PIC2_DATA, al            ; Slave bit 0 = IRQ8, bit 4 = IRQ12.
    sti                          ; Allow maskable CPU interrupts
    ret
