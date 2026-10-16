#include "io.h"

#define MASTER_PIC_COMMAND_PORT 0x20
#define MASTER_PIC_DATA_PORT 0x21
#define SLAVE_PIC_COMMAND_PORT 0xA0
#define SLAVE_PIC_DATA_PORT 0xA1

#define EXPECT_ICW4 0b00000001
#define SLAVE_ABSENT 0b00000010
#define LEVEL_TRIGGERED_PIC 0b00001000
#define INITIALIZE_PIC 0b00010000

#define SLAVE_CONNECTED_AT_IRQ0 0b00000001
#define SLAVE_CONNECTED_AT_IRQ1 0b00000010
#define SLAVE_CONNECTED_AT_IRQ2 0b00000100
#define SLAVE_CONNECTED_AT_IRQ3 0b00001000
#define SLAVE_CONNECTED_AT_IRQ4 0b00010000
#define SLAVE_CONNECTED_AT_IRQ5 0b00100000
#define SLAVE_CONNECTED_AT_IRQ6 0b01000000
#define SLAVE_CONNECTED_AT_IRQ7 0b10000000

#define MODE_8086 0b00000001
#define AUTO_END_OF_INTERRUPT 0b00000010

#define EOI 0b00100000

// Each PIC handles 8 IRQ inputs, but a PC needs more.
// The master handles IRQ0-IRQ7, the slave handles IRQ8-IRQ15.
// The slave's output connects to a master's IRQ input.
// The master forwards slave interrupt requests to the CPU.

// The PIC forwards hardware interrupts (IRQs) to the CPU.
// In protected mode, CPU exceptions use vectors 0-31, so we move
// hardware IRQs to vectors 32-47 to avoid conflicts.
//
// Master PIC: IRQ0-7  -> vectors 32-39
// Slave PIC:  IRQ8-15 -> vectors 40-47
//
// The CPU uses the resulting vector number to find the handler in the IDT.

void remap_pic() {
  // ICW1
  outb(MASTER_PIC_COMMAND_PORT, INITIALIZE_PIC | EXPECT_ICW4);
  outb(SLAVE_PIC_COMMAND_PORT, INITIALIZE_PIC | EXPECT_ICW4);

  // ICW2
  outb(MASTER_PIC_DATA_PORT, 32);
  outb(SLAVE_PIC_DATA_PORT, 40);

  // ICW3
  outb(MASTER_PIC_DATA_PORT, SLAVE_CONNECTED_AT_IRQ2);
  outb(SLAVE_PIC_DATA_PORT, 2);

  // ICW4 (optional, declared to be coming by ICW1)
  outb(MASTER_PIC_DATA_PORT, MODE_8086);
  outb(SLAVE_PIC_DATA_PORT, MODE_8086);
}

void end_of_interrupt(uint8_t irq) {
  if (irq >= 8) {
    outb(SLAVE_PIC_COMMAND_PORT, EOI);
  }
  outb(MASTER_PIC_COMMAND_PORT, EOI);
}
