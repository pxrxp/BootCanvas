NASM      = nasm
LD        = ld
OBJCOPY   = objcopy
QEMU      = qemu-system-x86_64
LDFLAGS   = -T linker.ld
BUILD_DIR = build
SRC_DIR   = src

.PHONY: all clean run

all: $(BUILD_DIR)/os.img

# Final disk image = boot sector + raw kernel binary
$(BUILD_DIR)/os.img: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin
	@echo "--- Creating final disk image: $@ ---"
	cat $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin > $@

# Bootloader (flat binary)
$(BUILD_DIR)/boot.bin: $(SRC_DIR)/boot.asm
	mkdir -p $(BUILD_DIR)
	@echo "--- Assembling bootloader ---"
	$(NASM) -f bin $< -o $@

# Assemble kernel parts as ELF objects
$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel.asm
	@echo "--- Assembling kernel ---"
	$(NASM) -f elf32 $< -o $@

$(BUILD_DIR)/gdt.o: $(SRC_DIR)/gdt.asm
	@echo "--- Assembling GDT ---"
	$(NASM) -f elf32 $< -o $@

# Link them into one ELF, then flatten it
$(BUILD_DIR)/kernel.elf: $(BUILD_DIR)/kernel.o $(BUILD_DIR)/gdt.o linker.ld
	@echo "--- Linking kernel ELF ---"
	$(LD) -m elf_i386 $(LDFLAGS) -o $@ $(BUILD_DIR)/kernel.o $(BUILD_DIR)/gdt.o

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	@echo "--- Converting kernel ELF → raw binary ---"
	$(OBJCOPY) -O binary --pad-to=4608 $< $@

run: $(BUILD_DIR)/os.img
	@echo "--- Running in QEMU ---"
	$(QEMU) -drive format=raw,file=$<

clean:
	rm -rf $(BUILD_DIR)

