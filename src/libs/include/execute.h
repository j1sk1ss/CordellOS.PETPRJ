#ifndef ELF_EXECUTE_H_
#define ELF_EXECUTE_H_

#include <stdint.h>
#include "stdlib.h"


struct ELF_program {
    uint32_t* pages;
    uint32_t pages_count;
    void* entry_point;
};


struct ELF_program* get_entry_point(char* path);
int execute(struct ELF_program* program, int argc, char* argv[]);
void free_program(struct ELF_program* program);

#endif