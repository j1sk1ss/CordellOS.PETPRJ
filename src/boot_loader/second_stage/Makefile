TARGET_ASMFLAGS += -f elf
TARGET_CFLAGS += -ffreestanding -nostdlib
TARGET_LIBS += -lgcc
TARGET_LINKFLAGS += -T linker.ld -nostdlib

SOURCE_C=$(wildcard *.c)
SOURCE_ASM=$(wildcard *.asm)

OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/second_stage/c/%.obj, $(SOURCE_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/second_stage/asm/%.obj, $(SOURCE_ASM))

.PHONY: all second_stage clean always

all: second_stage

second_stage: $(BUILD_DIR)/second_stage.bin

$(BUILD_DIR)/second_stage.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	$(TARGET_LD) $(TARGET_LINKFLAGS) -Wl,-Map=$(BUILD_DIR)/second_stage.map -o $@ $^

$(BUILD_DIR)/second_stage/c/%.obj: %.c always
	$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/second_stage/asm/%.obj: %.asm always
	$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<

always:
	mkdir -p $(BUILD_DIR)/second_stage/c
	mkdir -p $(BUILD_DIR)/second_stage/asm

clean:
	rm -f $(BUILD_DIR)/second_stage.bin