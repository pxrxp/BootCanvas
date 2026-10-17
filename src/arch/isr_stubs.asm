[bits 32]
global irq1_keyboard_stub
global irq12_mouse_stub
extern keyboard_handler
extern mouse_handler

irq1_keyboard_stub:
    pusha                   ; Save general-purpose registers
    call keyboard_handler   ; Call C handler
    popa                    ; Restore registers
    iret                    ; Return from interrupt
    
irq12_mouse_stub:
    pusha
    call mouse_handler
    popa
    iret
