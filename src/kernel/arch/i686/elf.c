#include "../../include/elf.h"


struct ELF32_program* ELF_read(const char* path) {

    struct ELF32_program* program = calloc(sizeof(struct ELF32_program), 1);

    struct FATContent* content = FAT_get_content(path);
    if (content->file == NULL) {
        kprintf("File not found\n");
        return NULL;
    }

    //==========================
    // Load ELF header
    //==========================

        void* header = calloc(sizeof(Elf32_Ehdr), 1);
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

        void* program_header = calloc(sizeof(Elf32_Phdr), ehdr->e_phnum);
        FAT_read_content2buffer(content, program_header, ehdr->e_phoff, sizeof(Elf32_Phdr) * ehdr->e_phnum);
        Elf32_Phdr* phdr = (Elf32_Phdr*)program_header;

        program->entry_point = ehdr->e_entry;
        uint32_t header_num  = ehdr->e_phnum;

        free(header);

    //==========================
    // Load program header
    //==========================
    // Copy data to vELF location
    //==========================

        program->pages       = calloc(header_num, sizeof(uint32_t));
        program->pages_count = header_num;
        for (uint32_t i = 0; i < header_num; i++) {
            if (phdr[i].p_type != PT_LOAD) continue;

            uint32_t program_pages    = phdr[i].p_memsz / PAGE_SIZE;
            uint32_t virtual_address  = phdr[i].p_vaddr;
            uint32_t physical_address = phdr[i].p_paddr;
            program->pages[i] = phdr[i].p_vaddr;

            if (phdr[i].p_memsz % PAGE_SIZE > 0) program_pages++;
            for (uint32_t i = 0; i < program_pages; i++) {
                pt_entry page  = physical_address;
                uint32_t* temp = allocate_page(&page);

                map_page((void*)temp, (void*)virtual_address);
                SET_ATTRIBUTE(&page, PTE_READ_WRITE);

                virtual_address  += PAGE_SIZE;
                physical_address += PAGE_SIZE;
            }

            memset(phdr[i].p_vaddr, 0, phdr[i].p_memsz);
            FAT_read_content2buffer(content, phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
        }

    //==========================
    // Copy data to vELF location
    //==========================

    free(program_header);
    FAT_unload_content_system(content);
    return program;
}