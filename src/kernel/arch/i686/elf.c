#include "../../include/elf.h"

void* ELF_exe_buffer;

void* ELF_read(const char* path) {
    struct FATContent* content = FAT_get_content(path);
    if (content->file == NULL) {
        kprintf("File not found\n");
        return NULL;
    }

    char* file_content = FAT_read_content(content);
    char* header_content = file_content;

    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)header_content;
    if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN) {
        kprintf("\r\nError: Program is not an executable or dynamic executable.\r\n");
        free(file_content);
        return NULL;
    }

    Elf32_Phdr* phdr = (Elf32_Phdr*)(header_content + ehdr->e_phoff);

    uint32_t mem_min   = 0xFFFFFFFF, mem_max = 0;
    uint32_t alignment = 4096;
    uint32_t align     = alignment;

    for (uint32_t i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD) continue;
        if (align < phdr[i].p_align) align = phdr[i].p_align;

        uint32_t mem_begin = phdr[i].p_vaddr;
        uint32_t mem_end = phdr[i].p_vaddr + phdr[i].p_memsz + align - 1;

        mem_begin &= ~(align - 1);
        mem_end &= ~(align - 1);

        if (mem_begin < mem_min) mem_min = mem_begin;
        if (mem_end > mem_max) mem_max = mem_end;
    }

    uint32_t buffer_size = mem_max - mem_min;
    ELF_exe_buffer = malloc(buffer_size);
    if (ELF_exe_buffer == NULL) {
        kprintf("\r\nError: Could not malloc() enough memory for program\r\n");
        free(file_content);
        return NULL;
    }

    memset(ELF_exe_buffer, 0, buffer_size);
    for (uint32_t i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD) continue;

        uint32_t relative_offset = phdr[i].p_vaddr - mem_min;

        uint8_t* dst = (uint8_t*)ELF_exe_buffer + relative_offset; 
        uint8_t* src = file_content + phdr[i].p_offset;
        uint32_t len = phdr[i].p_memsz;

        memcpy(dst, src, len);
    }

    free(file_content);
    FAT_unload_content_system(content);
    return (void*)((uint8_t*)ELF_exe_buffer + (ehdr->e_entry - mem_min));
}