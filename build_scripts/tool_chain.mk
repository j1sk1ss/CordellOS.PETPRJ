TOOLCHAIN_PREFIX = $(abspath tool_chain/$(TARGET))
export PATH := $(TOOLCHAIN_PREFIX)/bin:$(PATH)

tool_chain: tool_chain_binutils tool_chain_gcc

#
#	Binutils data
#
BINUTILS_SRC = tool_chain/binutils-$(BINUTILS_VERSION)
BINUTILS_BUILD = tool_chain/binutils-build-$(BINUTILS_VERSION)

tool_chain_binutils: $(TOOLCHAIN_PREFIX)/bin/i686-elf-ld

#
#	Binutils loading
#
$(TOOLCHAIN_PREFIX)/bin/i686-elf-ld: $(BINUTILS_SRC).tar.gz
	cd tool_chain && tar -xf binutils-$(BINUTILS_VERSION).tar.gz
	mkdir $(BINUTILS_BUILD)
	cd $(BINUTILS_BUILD) && ../binutils-$(BINUTILS_VERSION)/configure \
		--prefix="$(TOOLCHAIN_PREFIX)"	\
		--target=$(TARGET)				\
		--with-sysroot					\
		--disable-nls					\
		--disable-werror

	$(MAKE) -j8 -C $(BINUTILS_BUILD)
	$(MAKE) -C $(BINUTILS_BUILD) install

$(BINUTILS_SRC).tar.gz:
	mkdir -p tool_chain 
	cd tool_chain && wget $(BINUTILS_URL)

#
#	Compiler data
#
GCC_SRC = tool_chain/gcc-$(GCC_VERSION)
GCC_BUILD = tool_chain/gcc-build-$(GCC_VERSION)

tool_chain_gcc: $(TOOLCHAIN_PREFIX)/bin/i686-elf-gcc

#
#	Gcc compiler loading
#
$(TOOLCHAIN_PREFIX)/bin/i686-elf-gcc: $(TOOLCHAIN_PREFIX)/bin/i686-elf-ld $(GCC_SRC).tar.gz
	cd tool_chain && tar -xf gcc-$(GCC_VERSION).tar.gz
	mkdir $(GCC_BUILD)
	cd $(GCC_BUILD) && ../gcc-$(GCC_VERSION)/configure \
		--prefix="$(TOOLCHAIN_PREFIX)" 	\
		--target=$(TARGET)				\
		--disable-nls					\
		--enable-languages=c,c++		\
		--without-headers				

	$(MAKE) -j8 -C $(GCC_BUILD) all-gcc all-target-libgcc
	$(MAKE) -C $(GCC_BUILD) install-gcc install-target-libgcc
	
$(GCC_SRC).tar.gz:
	mkdir -p tool_chain
	cd tool_chain && wget $(GCC_URL)

#
# Clean
#
clean-toolchain:
	rm -rf $(GCC_BUILD) $(GCC_SRC) $(BINUTILS_BUILD) $(BINUTILS_SRC)

clean-toolchain-all:
	rm -rf tool_chain/*

.PHONY:  tool_chain tool_chain_binutils tool_chain_gcc clean-toolchain clean-toolchain-all