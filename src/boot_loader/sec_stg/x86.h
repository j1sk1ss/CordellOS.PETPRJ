#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ASMCALL __attribute__((cdecl))

//
//  Output data from address
//
void ASMCALL x86_outb(uint16_t port, uint8_t data);

//
//  Input data to address
//
uint8_t ASMCALL x86_inb(uint16_t port);

//
//  Get parameters of disk
//
bool ASMCALL x86_disk_getDriveParams(uint8_t drive,
                                                    uint8_t* driveTypeOut,
                                                    uint16_t* cylindersOut,
                                                    uint16_t* sectorsOut,
                                                    uint16_t* headersOut);

//
//  Reset disk
//
bool ASMCALL x86_diskReset(uint8_t drive);

//
//  Read disk
//
bool ASMCALL x86_diskRead(uint8_t drive,
                                        uint16_t cylinder,
                                        uint16_t sector,
                                        uint16_t head,
                                        uint8_t count,
                                        void* dataOut);

typedef struct {

    uint64_t Base;          //  Base Address Low : Low 32 bits of base address
    uint64_t Length;        //  Length Low : Low 32 bits of length bytes

    uint32_t Type;          //  Address type of range
    uint32_t ACPI;          //

} E820MemoryBlock;

enum E820MemoryBlockType {
    E820_USABLE             = 1,
    E820_RESERVED           = 2,
    E820_ACPI_RECLAIMABLE   = 3,
    E820_ACPI_NVS           = 4,
    E820_BAD_MEMORY         = 5
};

//
//
//
int ASMCALL x86_e820GetNextBlock(E820MemoryBlock* block, uint32_t continuationId);

//
//  Get bios VBE info
//
int ASMCALL x86_video_getVbeInfo(void* infoOut);

//
//  Get bios VBE mode info
//
int ASMCALL x86_video_getVbeModeInfo(uint16_t mode, void* infoOut);

//
//  Set bios VBE mode info
//
int ASMCALL x86_video_setVbeMode(uint16_t mode);