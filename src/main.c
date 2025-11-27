#include <stdint.h>

#define VIDEO_MEM 0xB8000

void kmain(void) {
    volatile uint16_t* mem = (volatile uint16_t*) VIDEO_MEM;

    for (uint32_t i = 0; i < 25; i++) {
        mem[i] = (0x1F << 8) | 'O';
    }

    while (1) {
        __asm__ __volatile__("hlt");
    }
}
