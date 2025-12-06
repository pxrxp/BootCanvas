section .data
align 4
global idtr

idtr:
    dw idt_end - idt_start - 1
    dd idt_start


section .bss
align 16
global idt_start
idt_start:
    resb 256 * 8          ; 256 entries, 8 bytes each
idt_end:


section .text
global set_idt_gate
extern GDT_CODE_SEL

set_idt_gate:
    push ebx

    mov ebx, eax                  ; ebx = handler
    movzx eax, al                 ; eax = vector
    shl eax, 3                    ; vector * 8
    add eax, idt_start            ; eax = &idt[vector]

    mov word  [eax + 0], bx       ; offset low
    mov word  [eax + 2], GDT_CODE_SEL
    mov byte  [eax + 4], 0
    mov byte  [eax + 5], 10001110b
    shr ebx, 16
    mov word  [eax + 6], bx       ; offset high

    pop ebx
    ret
