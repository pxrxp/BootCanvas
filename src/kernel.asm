section .text
global kstart

[bits 16]
kstart:
    ; Set the video memory pointer (ES:DI)
    mov ax, 0xb800          ; Video memory start segment
    mov es, ax              ; ES = 0xb800
    mov di, 0               ; DI = 0 (start at top-left corner)
    
    mov al, 'K'             ; Character 'K'
    mov ah, 0x24            ; Bright Green (0x2) on Black (0x0)
    stosw                   ; Store AX (word) into ES:[DI]
    
    jmp $
