NASM      = nasm
LD        = ld
GCC       = gcc
OBJCOPY   = objcopy
QEMU      = qemu-system-x86_64
BUILD_DIR = build
SRC_DIR   = src

GCC_INTERNAL_INC := $(shell $(GCC) -m32 -print-file-name=include)

CFLAGS    = -m32 -ffreestanding -fno-pic -fno-stack-protector -nostdinc \
            -I $(SRC_DIR) -I $(SRC_DIR)/libc/include -I $(GCC_INTERNAL_INC) -O0 -g
LDFLAGS   = -m elf_i386 -T linker.ld

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

ASM_SRCS := $(filter-out $(BOOT_SRC),$(shell find $(SRC_DIR) -type f -name '*.asm'))
C_SRCS   := $(shell find $(SRC_DIR) -type f -name '*.c')

ASM_OBJS := $(patsubst $(SRC_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SRCS))
C_OBJS   := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRCS))

# Pattern rule for assembling all ASM files automatically
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	mkdir -p $(dir $@)
	@echo "--- Assembling $< ---"
	$(NASM) -f elf32 $< -o $@

# Compile C kernel files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(GCC) $(CFLAGS) -c $< -o $@

# Then link all objects
$(KERNEL_ELF): $(ASM_OBJS) $(C_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Convert kernel ELF → raw binary
$(KERNEL_BIN): $(KERNEL_ELF)
	@echo "--- Converting kernel ELF → raw binary ---"
	$(OBJCOPY) -O binary $< $@

# Run in QEMU
run: $(OS_IMG)
	@echo "--- Running in QEMU ---"
	$(QEMU) -drive format=raw,file=$< -no-reboot -d int,cpu_reset -D $(BUILD_DIR)/qemu.log

# Clean
clean:
	@echo "--- Cleaning build files ---"
	rm -rf $(BUILD_DIR)
