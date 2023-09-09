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
	./build_scripts/make_floppy_another.sh $@

#
# Floppy Image
#

#
# Disk Image
#

disk_image: $(BUILD_DIR)/main_disk.raw 
$(BUILD_DIR)/main_disk.raw: boot_loader kernel
	./build_scripts/make_disk.sh $@ $(MAKE_DISK_SIZE)

#
# Disk Image
#

#
# Bootloader
#

boot_loader: fst_stg sec_stg

fst_stg: $(BUILD_DIR)/fst_stg.bin
$(BUILD_DIR)/fst_stg.bin: always
	$(MAKE) -C $(SRC_DIR)/boot_loader/fst_stg BUILD_DIR=$(abspath $(BUILD_DIR))

sec_stg: $(BUILD_DIR)/sec_stg.bin
$(BUILD_DIR)/sec_stg.bin: always
	$(MAKE) -C $(SRC_DIR)/boot_loader/sec_stg BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Bootloader
#

#
# Kernel
#

kernel: $(BUILD_DIR)/kernel.bin
$(BUILD_DIR)/kernel.bin: always
	$(MAKE) -C src/kernel BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Kernel
#

#
# Tools
#

tools_fat: $(BUILD_DIR)/tools/fat
$(BUILD_DIR)/tools/fat: always $(TOOLS_DIR)/fat/fat.c
	mkdir -p $(BUILD_DIR)/tools
	$(CC) -g -o $(BUILD_DIR)/tools/fat $(TOOLS_DIR)/fat/fat.c

#
# Tools
#

#
# Always
#

always:
	mkdir -p $(BUILD_DIR)

#
# Always
#

#
# Clean
#

clean:
	$(MAKE) -C $(SRC_DIR)/boot_loader/fst_stg BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	$(MAKE) -C $(SRC_DIR)/boot_loader/sec_stg BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	$(MAKE) -C $(SRC_DIR)/boot_loader/kernel BUILD_DIR=$(abspath $(BUILD_DIR)) clean

	rm -rf $(BUILD_DIR)/*

#
# Clean
#