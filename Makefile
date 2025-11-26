NASM      = nasm
LD        = ld
QEMU      = qemu-system-x86_64
LDFLAGS   = -T linker.ld
BUILD_DIR = build
SRC_DIR   = src

.PHONY: all clean run

all: $(BUILD_DIR)/os.img

$(BUILD_DIR)/os.img: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.elf
	@echo "--- Creating final disk image: $@ ---"
	cat $^ > $@

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/boot.asm
	@echo "--- Assembling bootloader: $@ ---"
	$(NASM) -f bin $< -o $@

$(BUILD_DIR)/kernel.elf: $(SRC_DIR)/kernel.asm
	@echo "--- Assembling kernel start: $@ ---"
	$(NASM) -f bin $< -o $@

$(BUILD_DIR)/linker.ld: $(SRC_DIR)/linker.ld
	cp $< $@

run: $(BUILD_DIR)/os.img
	@echo "--- Running in QEMU ---"
	$(QEMU) -drive format=raw,file=$<

clean:
	@echo "--- Cleaning build files ---"
	rm -rf $(BUILD_DIR)
