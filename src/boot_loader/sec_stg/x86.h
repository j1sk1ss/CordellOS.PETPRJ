#pragma once

#include <stdint.h>
#include <stdbool.h>

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t data);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);

bool __attribute__((cdecl)) x86_disk_getDeriveParams(uint8_t drive,
                                                    uint8_t* driveTypeOut,
                                                    uint16_t* cylindersOut,
                                                    uint16_t* sectorsOut,
                                                    uint16_t* headersOut);

bool __attribute__((cdecl)) x86_diskReset(uint8_t drive);

bool __attribute__((cdecl)) x86_diskRead(uint8_t drive,
                                        uint16_t cylinder,
                                        uint16_t sector,
                                        uint16_t head,
                                        uint8_t count,
                                        void* dataOut);