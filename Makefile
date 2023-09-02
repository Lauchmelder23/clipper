ASM=nasm

SRC_DIR=src
BUILD_DIR=build

BOOT_FILES 		= $(shell find $(SRC_DIR)/boot/ -type f -name "*.asm")
KERN_ASM_FILES 	= $(shell find $(SRC_DIR)/kernel/ -type f -name "*.asm")

BOOT_BIN_FILES 	= $(patsubst src/%.asm, $(BUILD_DIR)/%.bin, $(BOOT_FILES))
KERN_BIN_FILES 	= $(patsubst src/%.asm, $(BUILD_DIR)/%.bin, $(KERN_ASM_FILES))

BOOTLOADER 		= $(BUILD_DIR)/boot.bin
KERNEL 			= $(BUILD_DIR)/kernel.bin
IMAGE			= $(BUILD_DIR)/clipper.img

.PHONY: all clipper kernel bootloader clean always

clipper: $(IMAGE)

$(IMAGE): bootloader kernel
	dd if=/dev/zero of=$(IMAGE) bs=512 count=2880
	mkfs.fat -F 12 -n "CLPR" $(IMAGE)
	dd if=$(BOOTLOADER) of=$(IMAGE) conv=notrunc
	mcopy -i $(IMAGE) $(KERNEL) "::kernel.bin"

bootloader: $(BOOT_BIN_FILES)
	cp $(BUILD_DIR)/boot/boot.bin $(BOOTLOADER)

kernel: $(KERN_BIN_FILES)
	cp $(BUILD_DIR)/kernel/kernel.bin $(KERNEL)

$(BUILD_DIR)/%.bin: $(SRC_DIR)/%.asm
	@mkdir -p "$(@D)"
	$(ASM) $< -f bin -o $@

clean:
	rm -rf $(BUILD_DIR)/*
