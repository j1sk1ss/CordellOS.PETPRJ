#ifndef ELF_H_
#define ELF_H_


#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <math.h>
#include <fslib.h>

#include "elf.h"
#include "vfs.h"
#include "stdio.h"
#include "virt_manager.h"
#include "phys_manager.h"
#include "allocator.h"


#define EI_NIDENT (16)


typedef struct {
    unsigned char e_ident[EI_NIDENT];
    uint16_t	  e_type;
    uint16_t	  e_machine;
    uint32_t	  e_version;
    uint32_t	  e_entry;
    uint32_t	  e_phoff;
    uint32_t	  e_shoff;
    uint32_t	  e_flags;
    uint16_t	  e_ehsize;
    uint16_t	  e_phentsize;
    uint16_t	  e_phnum;
    uint16_t	  e_shentsize;
    uint16_t	  e_shnum;
    uint16_t	  e_shstrndx;

} Elf32_Ehdr;

// e_type values
enum {
    ET_NONE = 0x0,
    ET_REL,
    ET_EXEC,
    ET_DYN,
    // ...
};

typedef struct {
    uint32_t	p_type;
    uint32_t	p_offset;
    uint32_t	p_vaddr;
    uint32_t	p_paddr;
    uint32_t	p_filesz;
    uint32_t	p_memsz;
    uint32_t	p_flags;
    uint32_t	p_align;

} Elf32_Phdr;

// p_type values
enum {
    PT_NULL = 0x0,
    PT_LOAD,            // Loadable section
    // ...
};

typedef struct {
	uint32_t	sh_name;
	uint32_t	sh_type;
	uint32_t	sh_flags;
	uint32_t	sh_addr;
	uint32_t	sh_offset;
	uint32_t	sh_size;
	uint32_t	sh_link;
	uint32_t	sh_info;
	uint32_t	sh_addralign;
	uint32_t	sh_entsize;

} Elf32_Shdr;

typedef struct {
    uint32_t* pages;
    uint32_t pages_count;
    void* entry_point;
} ELF32_program;

enum ShT_Types {
	SHT_NULL	 = 0,   // Null section
	SHT_PROGBITS = 1,   // Program information
	SHT_SYMTAB	 = 2,   // Symbol table
	SHT_STRTAB	 = 3,   // String table
	SHT_RELA	 = 4,   // Relocation (w/ addend)
	SHT_NOBITS	 = 8,   // Not present in file
	SHT_REL		 = 9,   // Relocation (no addend)

};
 
enum ShT_Attributes {
	SHF_WRITE	= 0x01, // Writable section
	SHF_ALLOC	= 0x02  // Exists in memory

};


ELF32_program* ELF_read(const char* path, int type);


#endif