TARGET_ASMFLAGS += -f elf
TARGET_CFLAGS += -ffreestanding -nostdlib
TARGET_LIBS += -lgcc
TARGET_LINKFLAGS += -T linker.ld -nostdlib

SOURCE_C=$(wildcard *.c)
SOURCE_ASM=$(wildcard *.asm)

OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/sec_stg/c/%.obj, $(SOURCE_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/sec_stg/asm/%.obj, $(SOURCE_ASM))

.PHONY: all sec_stg clean always

all: sec_stg

sec_stg: $(BUILD_DIR)/sec_stg.bin

$(BUILD_DIR)/sec_stg.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	@$(TARGET_LD) $(TARGET_LINKFLAGS) -Wl,-Map=$(BUILD_DIR)/sec_stg.map -o $@ $^ $(TARGET_LIBS)
	@echo "--> Created  sec_stg.bin"

$(BUILD_DIR)/sec_stg/c/%.obj: %.c
	@mkdir -p $(@D)
	@$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<
	@echo "--> Compiled: " $<

$(BUILD_DIR)/sec_stg/asm/%.obj: %.asm
	@mkdir -p $(@D)
	@$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<
	@echo "--> Compiled: " $<

clean:
	@rm -f $(BUILD_DIR)/sec_stg.bin