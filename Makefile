NASM      = nasm
LD        = ld
GCC       = gcc
QEMU      = qemu-system-x86_64
BUILD_DIR = build
SRC_DIR   = src
LDFLAGS   = -T linker.ld

BOOT_BIN   = $(BUILD_DIR)/boot.bin
MAIN_O     = $(BUILD_DIR)/main.o
KERNEL_O   = $(BUILD_DIR)/kernel.o
GDT_O      = $(BUILD_DIR)/gdt.o
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
OS_IMG     = $(BUILD_DIR)/os.img

.PHONY: all clean run

all: $(OS_IMG)

# Create final disk image
$(OS_IMG): $(BOOT_BIN) $(KERNEL_ELF)
	@echo "--- Creating OS image: $@ ---"
	cat $^ > $@

# Assemble bootloader
$(BOOT_BIN): $(SRC_DIR)/boot.asm
	mkdir -p $(BUILD_DIR)
	@echo "--- Assembling bootloader ---"
	$(NASM) -f bin $< -o $@

# Assemble GDT
$(GDT_O): $(SRC_DIR)/gdt.asm
	mkdir -p $(BUILD_DIR)
	@echo "--- Assembling GDT ---"
	$(NASM) -f elf32 $< -o $@

# Assemble Kernel
$(KERNEL_O): $(SRC_DIR)/kernel.asm
	mkdir -p $(BUILD_DIR)
	@echo "--- Assembling Kernel---"
	$(NASM) -f elf32 $< -o $@


# Build all C source files into one object
$(MAIN_O): $(wildcard $(SRC_DIR)/*.c)
	mkdir -p $(BUILD_DIR)
	@echo "--- Compiling C kernel ---"
	$(GCC) -m32 -ffreestanding -fno-pic -O0 -g -c $< -o $@

# Link kernel ELF from Zig and GDT
$(KERNEL_ELF): $(KERNEL_O) $(MAIN_O) $(GDT_O) linker.ld
	@echo "--- Linking kernel ELF ---"
	$(LD) -m elf_i386 -T linker.ld -o $@ $(KERNEL_O) $(MAIN_O) $(GDT_O)

# Run in QEMU
run: $(OS_IMG)
	@echo "--- Running in QEMU ---"
	$(QEMU) -drive format=raw,file=$<

# Clean
clean:
	@echo "--- Cleaning build files ---"
	rm -rf $(BUILD_DIR)
