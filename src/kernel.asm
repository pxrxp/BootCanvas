section .text

extern gdtr
extern GDT_CODE_SEL
extern GDT_DATA_SEL

global kstart16
global kstart32
extern kmain

[bits 16]
kstart16:
    cli                           ; Disable interrupts.
    lgdt [gdtr]                   ; Load GDT register with start address of GDT

    mov ax, 0
    mov es, ax

    mov ax, 0x4F01                ; VESA Get Mode Info
    mov cx, 0x118                 ; mode 1024x768 24-bit
    lea di, [vesa_info]           ; store at vesa_info
    int 0x10
    jc fail

    mov ax, 0x4F02
    mov bx, 0x4118                ; 0x118 but linear framebuffer
    int 0x10
    jc fail

    mov eax, cr0
    or eax, 1                     ; Set Protected Enable (PE) bit
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

    mov eax, [vesa_info + 40]      ; framebuffer ptr
    mov bx, [vesa_info + 18]       ; width
    mov cx, [vesa_info + 20]       ; height
    mov dl, [vesa_info + 25]       ; bpp
    mov si, [vesa_info + 16]       ; pitch

    push esi
    push edx
    push ecx
    push ebx
    push eax
    call kmain

fail:
    jmp $

section .bss
align 16
vesa_info resb 256
