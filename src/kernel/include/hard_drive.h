#ifndef __HARDDRIVE_H__
#define __HARDDRIVE_H__

#include <stdint.h>

enum ATADEV {
    ATADEV_UNKNOWN  = 0,
    ATADEV_PATAPI   = 1,
    ATADEV_SATAPI   = 2,
    ATADEV_PATA     = 3,
    ATADEV_SATA     = 4
};

/**
 * On Primary bus: base = 0x1F0, dev_ctl = 0x3F6.
 */
uint8_t harddrive_detect_devtype(int slavebit, uint16_t base, uint16_t dev_ctl);

/**
 * Sector is in LBA28 format.
 */
uint8_t* hard_drive_load_sector(uint32_t sector);

const char* hard_drive_devtype_describe(uint8_t type);

#endif