BUILD_DIR?=build/
ASM?=nasm

.PHONY: all clean

all: fst_stg

fst_stg: $(BUILD_DIR)/fst_stg.bin

$(BUILD_DIR)/fst_stg.bin:
	$(ASM) boot.asm -f bin -o $(BUILD_DIR)/fst_stg.bin

clean:
	rm -f $(BUILD_DIR)/fst_stg.bin