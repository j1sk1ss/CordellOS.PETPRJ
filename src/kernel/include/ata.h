#include <stdint.h>

#include "../../libs/core/shared/include/x86.h"

#include "memory.h"
#include "string.h"
#include "stdio.h"

#define BOOT_SECTOR                 0
#define SECTOR_SIZE                 512
#define SECTOR_COUNT                1500

#define DATA_REGISTER               0x1F0
#define FEATURES_REGISTER           0x1F1
#define SECTOR_COUNT_REGISTER       0x1F2
#define LBA_ADRESS_REGISTER         0x1F3
#define CYLINDER_LOW_REGISTER       0x1F4
#define CYLINDER_HIGH_REGISTER      0x1F5
#define DRIVE_REGISTER              0x1F6
#define STATUS_REGISTER             0x1F7

#define ATA_SR_BSY                  0x8    // Busy

#define ATA_CMD_READ_PIO            0x20
#define ATA_CMD_WRITE_PIO           0x30

#define ATAPI_CMD_READ              0xA8
#define ATAPI_CMD_EJECT             0x1B

bool ATA_is_sector_empty(const uint8_t* sector_data);
bool ATA_is_current_sector_empty(uint32_t LBA);

char* ATA_read_sector(uint32_t LBA);
int ATA_write_sector(uint32_t lba, const uint8_t* buffer);

void ATA_append_sector(uint32_t lba, char* append_data);
void ATA_clear_sector(uint32_t LBA);

uint32_t ATA_find_empty_sector(uint32_t offset);
