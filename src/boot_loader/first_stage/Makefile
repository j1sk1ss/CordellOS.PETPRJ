BUILD_DIR?=build/
ASM?=nasm

.PHONY: all clean

all: first_stage

first_stage: $(BUILD_DIR)/first_stage.bin

$(BUILD_DIR)/first_stage.bin:
	$(ASM) boot.asm -f bin -o $(BUILD_DIR)/first_stage.bin

clean:
	rm -f $(BUILD_DIR)/first_stage.bin