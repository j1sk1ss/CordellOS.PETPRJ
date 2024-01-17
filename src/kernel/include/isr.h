#pragma once

#include <stdint.h>

// Screen managment through kprint 
#define SYS_PRINT            0
#define SYS_PUTC             1
#define SYS_CLEAR            2
#define SYS_COLOR_PUTC       13
#define SYS_SCREEN_COLOR     14

// System managment (memory, tasks)
#define SYS_SLEEP            3 
#define SYS_READ_KEYBOARD    4
#define SYS_GET_KEY_KEYBOARD 5
#define SYS_TIME             6
#define SYS_MALLOC           7
#define SYS_FREE             8

// FS managment (FAT32)
#define SYS_READ_FILE        9
#define SYS_WRITE_FILE       10
#define SYS_OPENDIR          11
#define SYS_EXECUTE_FILE     12 // Not implemented yet
#define SYS_CEXISTS          15 // Not implemented yet
#define SYS_FCREATE          16 // Not implemented yet
#define SYS_DIRCREATE        17 // Not implemented yet
#define SYS_CDELETE          18 // Not implemented yet


typedef struct {
    uint32_t ds;                                            // data segment pushed by us
    uint32_t edi, esi, ebp, kern_esp, ebx, edx, ecx, eax;   // pusha
    uint32_t interrupt, error;                              // we push interrupt and error code
    uint32_t eip, cs, eflag, esp, ss;                       // ushed auto by cpu
} __attribute__((packed)) Registers;

typedef void (*ISRHandler)(Registers* regs);

void i686_isr_initialize();
void i686_isr_registerHandler(int interrupt, ISRHandler handler);
