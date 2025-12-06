NASM      = nasm
LD        = ld
GCC       = gcc
OBJCOPY   = objcopy
QEMU      = qemu-system-x86_64
BUILD_DIR = build
SRC_DIR   = src
LDFLAGS   = -T linker.ld

BOOT_BIN   = $(BUILD_DIR)/boot.bin
BOOT_TMP   = $(BUILD_DIR)/boot_fixed.asm
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
OS_IMG     = $(BUILD_DIR)/os.img

BOOT_SRC   = $(SRC_DIR)/boot.asm

.PHONY: all clean run

all: $(OS_IMG)

# Create final OS image
$(OS_IMG): $(BOOT_BIN) $(KERNEL_BIN)
	@echo "--- Creating OS image: $@ ---"
	cat $^ > $@

# Assemble bootloader with correct sector count
$(BOOT_BIN): $(BOOT_SRC) $(KERNEL_BIN)
	mkdir -p $(BUILD_DIR)
	@echo "--- Generating bootloader with correct sector count ---"
	SECTORS=$$(( ( $$(stat -c%s $(KERNEL_BIN)) + 511 ) / 512 )) ; \
	echo "Sectors = $$SECTORS" ; \
	sed "s/__NUM_SECTORS__/$${SECTORS}/" $(BOOT_SRC) > $(BOOT_TMP)
	@echo "--- Assembling bootloader ---"
	$(NASM) -f bin $(BOOT_TMP) -o $@

ASM_SRCS := $(filter-out $(SRC_DIR)/boot.asm,$(wildcard $(SRC_DIR)/*.asm))
ASM_OBJS := $(patsubst $(SRC_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SRCS))

# Pattern rule for assembling all ASM files automatically
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	mkdir -p $(BUILD_DIR)
	@echo "--- Assembling $< ---"
	$(NASM) -f elf32 $< -o $@

# Compile C kernel files
C_SRCS := $(wildcard $(SRC_DIR)/*.c)
C_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRCS))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(GCC) -m32 -ffreestanding -fno-pic -fno-stack-protector -O0 -g -c $< -o $@

# Then link all objects
$(KERNEL_ELF): $(ASM_OBJS) $(C_OBJS)
	$(LD) -m elf_i386 -T linker.ld -o $@ $^

# Convert kernel ELF → raw binary
$(KERNEL_BIN): $(KERNEL_ELF)
	@echo "--- Converting kernel ELF → raw binary ---"
	$(OBJCOPY) -O binary $< $@

# Run in QEMU
run: $(OS_IMG)
	@echo "--- Running in QEMU ---"
	$(QEMU) -drive format=raw,file=$<

# Clean
clean:
	@echo "--- Cleaning build files ---"
	rm -rf $(BUILD_DIR)

