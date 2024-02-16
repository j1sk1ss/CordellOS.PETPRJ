#include "../../include/elf.h"

void* ELF_exe_buffer;

void* ELF_read(const char* path) {

    struct FATContent* content = FAT_get_content(path);
    if (content->file == NULL) {
        kprintf("File not found\n");
        return NULL;
    }

    //==========================
    // Load ELF header
    //==========================

        void* header = malloc(sizeof(Elf32_Ehdr));
        FAT_read_content2buffer(content, header, 0, sizeof(Elf32_Ehdr));
        Elf32_Ehdr* ehdr = (Elf32_Ehdr*)header;
        if (ehdr->e_ident[0] != '\x7f' || ehdr->e_ident[1] != 'E') {
            kprintf("\r\nError: Not ELF.\r\n");
            free(header);
            return NULL;
        }

        if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN) {
            kprintf("\r\nError: Program is not an executable or dynamic executable.\r\n");
            free(header);
            return NULL;
        }

    //==========================
    // Load ELF header
    //==========================
    // Load program header
    //==========================

        void* program_header = malloc(sizeof(Elf32_Phdr));
        FAT_read_content2buffer(content, program_header, ehdr->e_phoff, sizeof(Elf32_Phdr));
        Elf32_Phdr* phdr = (Elf32_Phdr*)program_header;

        uint32_t program_entry = ehdr->e_entry;
        uint32_t header_num    = ehdr->e_phnum;
        uint32_t mem_min       = 0xFFFFFFFF, mem_max = 0;
        uint32_t alignment     = PAGE_SIZE;
        uint32_t align         = alignment;

        free(header);

    //==========================
    // Load program header
    //==========================
    // Size calculation
    //==========================

        for (uint32_t i = 0; i < header_num; i++) {
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
        uint32_t buffer_alignment = align - 1;

        int pages = buffer_size / PAGE_SIZE;
        if (buffer_size % PAGE_SIZE > 0) pages++;

        //==========================
        // Buffer & pages allocation
        //==========================

            ELF_exe_buffer           = (void*)ELF_VIRT_LOCATION;
            uint32_t virtual_address = ELF_VIRT_LOCATION;

            for (uint8_t i = 0; i < pages; i++) {
                pt_entry page  = 0;
                uint32_t* temp = allocate_page(&page);

                map_page((void*)temp, (void*)virtual_address);
                SET_ATTRIBUTE(&page, PTE_READ_WRITE);

                virtual_address += PAGE_SIZE;
            }

            memset(ELF_exe_buffer, 0, sizeof(buffer_size));

            if (ELF_exe_buffer == NULL) {
                kprintf("\r\nError: Could not allocate enough memory for program\r\n");
                free(program_header);
                return NULL;
            }

        //==========================
        // Buffer & pages allocation
        //==========================

    //==========================
    // Size calculation
    //==========================
    // Copy data to ELF location
    //==========================

        for (uint32_t i = 0; i < header_num; i++) {
            if (phdr[i].p_type != PT_LOAD) continue;

            uint32_t relative_offset = phdr[i].p_vaddr - mem_min;
            uint32_t len = phdr[i].p_memsz;

            uint8_t* src = malloc(len);
            uint8_t* dst = (uint8_t*)ELF_exe_buffer + relative_offset;
            FAT_read_content2buffer(content, src, phdr[i].p_offset, len);

            memcpy(dst, src, len);
            free(src);
        }

    //==========================
    // Copy data to ELF location
    //==========================

    free(program_header);
    FAT_unload_content_system(content);
    return (void*)((uint8_t*)ELF_exe_buffer + (program_entry - mem_min));
}