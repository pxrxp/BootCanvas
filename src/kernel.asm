section .text

extern gdtr
extern GDT_CODE_SEL
extern GDT_DATA_SEL

global kstart
global kmain

[bits 16]
kstart:
    cli            ; Disable interrupts.
    lgdt [gdtr]    ; Load GDT register with start address of GDT
    mov eax, cr0
    or al, 1       ; Set Protected Enable (PE) bit
    mov cr0, eax
    jmp GDT_CODE_SEL:kmain

[bits 32]
kmain:
    mov ax, GDT_DATA_SEL
    mov ds, ax 
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    mov edi, 0x00             ; EDI = Video memory offset
    mov eax, 0xb8000          ; EAX = Video memory address
    mov ecx, 25               ; Count for loop
    mov byte [eax+edi], '0'
    mov byte [eax+edi+1], 0x1F ; white on cyan

    jmp $
