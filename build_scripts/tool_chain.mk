TOOLCHAIN_PREFIX = $(abspath tool_chain/$(TARGET))
export PATH := $(TOOLCHAIN_PREFIX)/bin:$(PATH)

tool_chain: tool_chain_binutils tool_chain_gcc

BINUTILS_SRC = tool_chain/binutils-$(BINUTILS_VERSION)
BINUTILS_BUILD = tool_chain/binutils-build-$(BINUTILS_VERSION)

tool_chain_binutils: $(TOOLCHAIN_PREFIX)/bin/i686-elf-ld

$(TOOLCHAIN_PREFIX)/bin/i686-elf-ld: $(BINUTILS_SRC).tar.xz
	cd tool_chain && tar -xf binutils-$(BINUTILS_VERSION).tar.xz
	mkdir $(BINUTILS_BUILD)
	cd $(BINUTILS_BUILD) && CFLAGS= ASMFLAGS= CC= CXX= LD= ASM= LINKFLAGS= LIBS= ../binutils-$(BINUTILS_VERSION)/configure \
		--prefix="$(TOOLCHAIN_PREFIX)"	\
		--target=$(TARGET)				\
		--with-sysroot					\
		--disable-nls					\
		--disable-werror
	$(MAKE) -j8 -C $(BINUTILS_BUILD)
	$(MAKE) -C $(BINUTILS_BUILD) install

$(BINUTILS_SRC).tar.xz:
	mkdir -p tool_chain 
	cd tool_chain && wget $(BINUTILS_URL)


GCC_SRC = tool_chain/gcc-$(GCC_VERSION)
GCC_BUILD = tool_chain/gcc-build-$(GCC_VERSION)

tool_chain_gcc: $(TOOLCHAIN_PREFIX)/bin/i686-elf-gcc

$(TOOLCHAIN_PREFIX)/bin/i686-elf-gcc: $(TOOLCHAIN_PREFIX)/bin/i686-elf-ld $(GCC_SRC).tar.xz
	cd tool_chain && tar -xf gcc-$(GCC_VERSION).tar.xz
	mkdir $(GCC_BUILD)
	cd $(GCC_BUILD) && CFLAGS= ASMFLAGS= CC= CXX= LD= ASM= LINKFLAGS= LIBS= ../gcc-$(GCC_VERSION)/configure \
		--prefix="$(TOOLCHAIN_PREFIX)" 	\
		--target=$(TARGET)				\
		--disable-nls					\
		--enable-languages=c,c++		\
		--without-headers
	$(MAKE) -j8 -C $(GCC_BUILD) all-gcc all-target-libgcc
	$(MAKE) -C $(GCC_BUILD) install-gcc install-target-libgcc
	
$(GCC_SRC).tar.xz:
	mkdir -p tool_chain
	cd tool_chain && wget $(GCC_URL)

#
# Clean
#
clean-tool_chain:
	rm -rf $(GCC_BUILD) $(GCC_SRC) $(BINUTILS_BUILD) $(BINUTILS_SRC)

clean-tool_chain-all:
	rm -rf tool_chain/*

.PHONY:  tool_chain tool_chain_binutils tool_chain_gcc clean-tool_chain clean-tool_chain-all