[bits 32]
global irq1_keyboard_stub
extern keyboard_handler

irq1_keyboard_stub:
    pusha                   ; Save general-purpose registers
    call keyboard_handler   ; Call C handler
    popa                    ; Restore registers
    iret                    ; Return from interrupt
