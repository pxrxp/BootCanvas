section .text

extern gdtr
extern GDT_CODE_SEL
extern GDT_DATA_SEL

global kstart16
global kstart32
extern kmain

[bits 16]
kstart16:
    cli             ; Disable interrupts.
    lgdt [gdtr]     ; Load GDT register with start address of GDT
    mov eax, cr0
    or eax, 1       ; Set Protected Enable (PE) bit
    mov cr0, eax
    jmp GDT_CODE_SEL:kstart32

[bits 32]
kstart32:
    mov ax, GDT_DATA_SEL
    mov ds, ax 
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    call kmain

    jmp $
