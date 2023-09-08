#pragma once

#include <stdint.h>
#include <stdbool.h>

//
//  Output data from address
//
void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t data);

//
//  Input data to address
//
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);

//
//  Get parameters of disk
//
bool __attribute__((cdecl)) x86_disk_getDriveParams(uint8_t drive,
                                                    uint8_t* driveTypeOut,
                                                    uint16_t* cylindersOut,
                                                    uint16_t* sectorsOut,
                                                    uint16_t* headersOut);

//
//  Reset disk
//
bool __attribute__((cdecl)) x86_diskReset(uint8_t drive);

//
//  Read disk
//
bool __attribute__((cdecl)) x86_diskRead(uint8_t drive,
                                        uint16_t cylinder,
                                        uint16_t sector,
                                        uint16_t head,
                                        uint8_t count,
                                        void* dataOut);

//
//  Get bios VBE info
//
int __attribute__((cdecl)) x86_video_getVbeInfo(void* infoOut);

//
//  Get bios VBE mode info
//
int __attribute__((cdecl)) x86_video_getVbeModeInfo(uint16_t mode, void* infoOut);

//
//  Set bios VBE mode info
//
int __attribute__((cdecl)) x86_video_setVbeMode(uint16_t mode);