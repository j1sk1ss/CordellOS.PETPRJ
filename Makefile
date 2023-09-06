SRC_DIR=src
TOOLS_DIR=tools

include build_scripts/config.mk 

.PHONY: all floppy_image kernel boot_loader clean always tools_fat

all: floppy_image tools_fat

include build_scripts/tool_chain.mk 

#
# Floppy Image
#
floppy_image: $(BUILD_DIR)/main_floppy.img 

$(BUILD_DIR)/main_floppy.img: boot_loader kernel
	dd if=/dev/zero of=$(BUILD_DIR)/main_floppy.img bs=512 count=2880
	newfs_msdos -F 12 -f 2880 $(BUILD_DIR)/main_floppy.img
	dd if=$(BUILD_DIR)/first_stage.bin of=$(BUILD_DIR)/main_floppy.img conv=notrunc

	mcopy -i $(BUILD_DIR)/main_floppy.img $(BUILD_DIR)/second_stage.bin "::second_stage.bin"
	mcopy -i $(BUILD_DIR)/main_floppy.img $(BUILD_DIR)/kernel.bin "::kernel.bin"
	mcopy -i $(BUILD_DIR)/main_floppy.img test.txt "::test.txt"

#
# Bootloader
#
boot_loader: first_stage second_stage

first_stage: $(BUILD_DIR)/first_stage.bin
$(BUILD_DIR)/first_stage.bin: always
	$(MAKE) -C $(SRC_DIR)/boot_loader/first_stage BUILD_DIR=$(abspath $(BUILD_DIR))

second_stage: $(BUILD_DIR)/second_stage.bin
$(BUILD_DIR)/second_stage.bin: always
	$(MAKE) -C $(SRC_DIR)/boot_loader/second_stage BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Kernel
#
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	$(ASM) $(SRC_DIR)/kernel -f bin -o $(BUILD_DIR)/kernel.bin

#
# Tools
#
tools_fat: $(BUILD_DIR)/tools/fat
$(BUILD_DIR)/tools/fat: always $(TOOLS_DIR)/fat/fat.c
	mkdir -p $(BUILD_DIR)/tools
	$(CC) -g -o $(BUILD_DIR)/tools/fat $(TOOLS_DIR)/fat/fat.c

#
# Always
#
always:
	mkdir -p $(BUILD_DIR)

#
# Clean
#
clean:
	$(MAKE) -C $(SRC_DIR)/boot_loader/first_stage BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	$(MAKE) -C $(SRC_DIR)/boot_loader/second_stage BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	$(MAKE) -C $(SRC_DIR)/boot_loader/kernel BUILD_DIR=$(abspath $(BUILD_DIR)) clean

	rm -rf $(BUILD_DIR)/*
