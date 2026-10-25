<p align="center">
<img width="512" height="128" alt="image" src="https://github.com/user-attachments/assets/308702c4-c6c0-4257-a42c-a6a3c5808376" />
</p>

BootCanvas is a bare-metal x86 operating system kernel built from scratch in C and assembly. It boots via BIOS, enters protected mode, initializes a linear framebuffer, and provides a simple graphical environment with a console, keyboard/mouse input, and small GUI programs.

This project is not a general-purpose Linux-like OS. It is a compact kernel-focused environment designed to explore low-level bootstrapping, protected mode setup, interrupt handling, graphics primitives, and user-interface code running directly on hardware.

https://github.com/user-attachments/assets/66eeacd7-2af7-490e-8749-0f70b5db0224

This is a screen recording of BootCanvas running on QEMU, a free and open-source machine emulator.

## Project summary

BootCanvas combines several low-level pieces into a functioning bootable kernel image:

- BIOS boot sector that loads the kernel from disk
- 32-bit protected mode startup
- Global Descriptor Table (GDT) setup
- Interrupt Descriptor Table (IDT) setup
- PIC remapping for hardware IRQs
- PS/2 keyboard and mouse drivers
- VESA framebuffer graphics
- 2D drawing primitives: pixels, lines, rectangles, ellipses, text
- Console shell with commands
- Windowed GUI programs, including a paint-style app and a calculator GUI program
- A minimal custom libc subset for kernel code

BootCanvas focuses on the kernel rather than a general-purpose desktop environment.

## System architecture

The repository is organized as follows:

```text
BootCanvas/
├── Makefile
├── linker.ld
├── README.md
├── LICENSE
├── .gitignore
├── src/
│   ├── boot.asm                 # BIOS bootloader stage
│   ├── kernel.asm              # Protected mode kernel entry and VESA setup
│   ├── main.c                  # Kernel entry point
│   ├── arch/
│   │   ├── gdt.asm             # GDT definitions
│   │   ├── idt.asm             # IDT setup and interrupt gate loading
│   │   ├── isr_stubs.asm       # Keyboard and mouse interrupt stubs
│   │   ├── interrupts.c        # PIC remap / EOI handling
│   │   ├── interrupts.h
│   │   ├── io.h                # inb/outb helpers and HLT wrapper
│   │   ├── program.c           # current program dispatcher
│   │   └── program.h
│   ├── drivers/
│   │   ├── font.h              # 8x8 bitmap font data
│   │   ├── graphics.c          # framebuffer and drawing functions
│   │   ├── graphics.h
│   │   ├── keyboard.c          # PS/2 keyboard handling
│   │   ├── keyboard.h
│   │   ├── mouse.c             # PS/2 mouse packet parsing
│   │   ├── mouse.h
│   │   └── scancode.h
│   ├── gui/
│   │   ├── gui.c               # UI helpers and buttons
│   │   └── gui.h
│   ├── libc/
│   │   ├── conio.c             # console I/O for kernel
│   │   ├── ctype.c
│   │   ├── math.c
│   │   ├── stdlib.c
│   │   ├── string.c
│   │   └── include/
│   │       ├── conio.h
│   │       ├── ctype.h
│   │       ├── math.h
│   │       ├── stdlib.h
│   │       └── string.h
│   ├── programs/
│   │   ├── console.c           # shell and command dispatcher
│   │   ├── console.h
│   │   ├── paint_gui.c        # paint-like drawing app
│   │   ├── paint_gui.h
│   │   ├── calc_gui.c         # calculator app
│   │   ├── calc_gui.h
│   │   ├── mouse.c             # mouse-related program utilities
│   │   └── mouse.h
│   └── main.c                  # entry and event loop
└── linker.ld                   # ELF linker script for kernel
```

## Boot flow

The boot sequence is:

1. BIOS loads the first sector into memory at `0x7C00`.
2. `src/boot.asm` runs in real mode.
3. It:
   - saves the boot drive number from `DL`
   - clears segment registers
   - sets up a stack
   - enables A20
   - uses BIOS `INT 13h` to read the kernel from disk
   - jumps to `0x0000:0x1000`
4. The kernel is loaded after the bootloader sector, and control transfers into the kernel entry.
5. `src/kernel.asm` then:
   - loads the GDT
   - queries VESA mode info
   - switches to 1024x768, 24-bit color mode
   - enables protected mode
   - sets CR0 and CR4 bits for protected mode and SSE
   - sets up the stack
   - calls `kmain`

The linker script (`linker.ld`) defines the kernel layout and sets the entry point to `kstart16`, with the runtime memory image spanning from `0x1000` upward.

## Protected mode and CPU state

The kernel uses a flat memory model with a 32-bit protected mode environment.

Important kernel startup details:

- `GDT_CODE_SEL` and `GDT_DATA_SEL` are used to move from real mode to protected mode
- `cr0` has the PE bit set to enable protected mode
- `cr4` has `OSFXSR` and `OSXMMEXCPT` set to enable SSE/SSE2
- `fninit` resets x87 floating-point state
- `lgdt` loads the global descriptor table
- `lidt` is used later to install the interrupt descriptor table

The kernel configures CPU state directly during startup.

## Graphics subsystem

The graphics system is built around a linear framebuffer provided by VESA BIOS extensions.

The kernel obtains:

- framebuffer pointer
- width
- height
- bits per pixel
- pitch

This is passed into `init_graphics()` from `src/main.c` and stored in a `GraphicsState` struct.

Relevant graphics primitives include:

- `plot_pixel()`
- `get_pixel()`
- `line()`
- `rectangle()`
- `filled_rectangle()`
- `ellipse()`
- `put_char()`
- `put_text()`
- `clear()`
- `swap_buffers()`

The graphics driver stores a backbuffer in memory and copies it to the framebuffer on each frame tick. This allows drawing operations to be staged before visible presentation.

The project also includes a simple bitmap font in `src/drivers/font.h`, which is used to render text directly to the framebuffer.

## Input and interrupts

The kernel configures the chipset interrupt system directly:

- keyboard IRQ is mapped to vector `0x21`
- mouse IRQ is mapped to vector `0x2C`
- the master and slave PIC are remapped away from the CPU exception vectors
- the IDT is loaded with handlers for the enabled IRQs

The interrupt handling path is:

- `setup_interrupts()` in `src/arch/idt.asm`
- `remap_pic()` in `src/arch/interrupts.c`
- `irq1_keyboard_stub` / `irq12_mouse_stub` in `src/arch/isr_stubs.asm`
- `keyboard_handler()` / `mouse_handler()`

### Keyboard

Keyboard support is implemented with standard PS/2 controller communication:

- controller input/output port access via `0x64` and `0x60`
- scan-code decoding
- modifier tracking for:
  - shift
  - control
  - alt
  - caps lock
- conversion to ASCII with shifted/unshifted mappings
- event toggling to detect input changes without polling too aggressively

The keyboard driver tracks:

- last scancode
- key pressed state
- event_toggle

and exposes a `KeyboardState` structure.

### Mouse

The mouse driver parses standard PS/2 packet data:

- left button
- right button
- middle button
- x delta
- y delta

Mouse movement is clamped to the current framebuffer dimensions and stored in `MouseState`:

```c
typedef struct {
  int32_t x;
  int32_t y;
  bool left_button;
  bool middle_button;
  bool right_button;
} MouseState;
```

The main kernel loop polls for hardware changes and then calls the active program.

## Kernel runtime model

`src/main.c` is the primary kernel entry point:

```c
void kmain(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp,
           uint32_t pitch) {
  init_graphics(framebuffer, width, height, bpp, pitch);
  init_console();
  setup_interrupts();

  init_program();
  set_current_program((Program){NULL, CONSOLE_run});

  while (1) {
    wait_for_interrupts();
    MOUSE_restore_bg();
    run_program();
    MOUSE_run();
    swap_buffers();
  }
}
```

The project uses a simple program-switching model:

- `Program` contains:
  - `init()`
  - `run(bool keyboard_changed, bool mouse_changed)`
- `set_current_program()` swaps the active runtime program
- `run_program()` checks for keyboard/mouse changes and calls the active program

This allows the kernel to switch between a shell and GUI programs without any real OS scheduler.

## Console shell

<img width="566" height="190" alt="image" src="https://github.com/user-attachments/assets/da394d20-f40c-4753-a7ac-e73d828daa16" />

The console is the first active program launched after the kernel initializes.

The shell supports commands such as:

- `help`
- `clear` / `cls`
- `echo <text>`
- `about`
- `paint` / `draw`
- `calc` / `calculator`

The console is implemented in `src/programs/console.c` and uses a custom kernel console I/O layer.

Examples from the code:

- `help` prints the command list
- `about` prints the kernel version and display resolution
- `paint` switches to the paint GUI program
- `calc` switches to the calculator GUI program

The shell is a minimal command environment, not a full TTY or Unix shell.

## Paint program

<img width="711" height="538" alt="image" src="https://github.com/user-attachments/assets/d490652c-cbb5-4984-908f-e7eb6347a2a4" />

The paint-style program is implemented in `src/programs/paint_gui.c`.

It provides:

- tool sidebar
- color palette
- pencil tool
- line tool
- rectangle tool
- ellipse tool
- clear canvas
- exit to shell

The drawing area is defined by:

```c
static Point g_canvas_tl = {170, 10};
static Point g_canvas_br = {790, 590};
```

The app uses the same framebuffer drawing primitive layer as the rest of the kernel and is event-driven from mouse state.

The UI is composed using helper functions in `src/gui/gui.c`, including:

- `ui_draw_panel()`
- `ui_draw_window_frame()`
- `ui_draw_button()`
- `ui_button_clicked()`

The paint program uses these helpers to draw its panels, window frame, and buttons.

## Calculator program

<img width="321" height="384" alt="image" src="https://github.com/user-attachments/assets/f0c6bac0-4d7a-4d5c-8611-296b8f646d85" />

A calculator GUI program exists in:

- `src/programs/calc_gui.c`
- `src/programs/calc_gui.h`

It is launched from the console with:

- `calc`
- `calculator`

The calculator runs as a kernel program and is switched to from the console.

## Custom libc subset

The kernel includes a small custom standard library layer for low-level C code:

- `src/libc/ctype.c` and `ctype.h`
- `src/libc/string.c` and `string.h`
- `src/libc/stdlib.c` and `stdlib.h`
- `src/libc/conio.c` and `conio.h`
- `src/libc/math.c` and `math.h`

This is not a standard libc implementation. It is a compact set of utilities intended for kernel-level development.

Notable functions include:

- `itoa()`
- string comparison / length utilities
- `isspace()`-style classification
- console output helpers
- numeric conversion utilities

The build uses these headers as part of the kernel compilation pipeline.

## Linker and build system

The project is built using `Makefile`.

Key build targets:

- `make` or `make all` builds the OS image
- `make run` launches the OS in QEMU
- `make clean` removes the build directory
- `make compile_commands.json` generates compilation metadata when `compiledb` is installed

The build creates:

- `build/boot.bin`
- `build/kernel.elf`
- `build/kernel.bin`
- `build/os.img`

The bootloader uses `sed` to patch the sector count into `boot.asm` dynamically:

```make
SECTORS=$$(( ( $$(stat -c%s $(KERNEL_BIN)) + 511 ) / 512 )) ; \
sed "s/__NUM_SECTORS__/$${SECTORS}/" $(BOOT_SRC) > $(BOOT_TMP)
```

This patches the kernel's sector count into the bootloader so the BIOS reads the required number of sectors.

## Build and run

Requirements:

- NASM
- GCC in 32-bit mode (`-m32`)
- `ld`
- `objcopy`
- QEMU (`qemu-system-x86_64`)

Build:

```bash
git clone https://github.com/pxrxp/BootCanvas.git
cd BootCanvas
make
```

Run in QEMU:

```bash
make run
```

This runs:

```bash
qemu-system-x86_64 -drive format=raw,file=build/os.img -no-reboot -d int,cpu_reset -D build/qemu.log
```

If you want to start directly without the make target:

```bash
qemu-system-x86_64 -drive format=raw,file=build/os.img
```

## Current limitations

The system currently provides:

- raw kernel boot
- protected mode CPU startup
- direct hardware access
- framebuffer graphics
- keyboard and mouse drivers
- a small command shell
- simple GUI programs

It does not currently provide:

- paging
- virtual memory
- preemptive multitasking
- file system
- network stack
- a true user-space process model
- a robust kernel API
- a fully general-purpose desktop environment

## License

This project is distributed under the MIT License. See `LICENSE` for details.
