#include "../../include/ata.h"

///////////////////////////////////////////
//
//  READ SECTOR FROM DISK BY LBA ADRESS
//
    uint8_t* ATA_read_sector(uint32_t lba) {
        ATA_ata_wait();
        uint8_t* buffer = (uint8_t*)malloc(SECTOR_SIZE);
        if (buffer == NULL) return NULL;

        i386_outb(DRIVE_REGISTER, 0xE0 | (0x00 << 4) | ((lba >> 24) & 0x0F));
        i386_outb(FEATURES_REGISTER, 0x00);
        i386_outb(SECTOR_COUNT_REGISTER, 1);
        i386_outb(LBA_ADRESS_REGISTER, (uint8_t)(lba & 0xFF));
        i386_outb(CYLINDER_LOW_REGISTER, (uint8_t)((lba >> 8) & 0xFF));
        i386_outb(CYLINDER_HIGH_REGISTER, (uint8_t)((lba >> 16) & 0xFF));
        i386_outb(STATUS_REGISTER, ATA_CMD_READ_PIO);

        int timeout = 9000000;
        while ((i386_inb(STATUS_REGISTER) & ATA_SR_BSY) == 0) 
            if (--timeout < 0) {
                free(buffer);
                return NULL;
            }
            else continue;

        for (int n = 0; n < SECTOR_SIZE / 2; n++) {
            uint16_t value = i386_inw(DATA_REGISTER);
            buffer[n * 2] = value & 0xFF;
            buffer[n * 2 + 1] = value >> 8;
        }

        return buffer;
    }

    // Function to read a sectors from the disk.
    uint8_t* ATA_read_sectors(uint32_t lba, uint32_t sector_count) {
        ATA_ata_wait();
        uint8_t* buffer = (uint8_t*)malloc(SECTOR_SIZE * sector_count);
        if (buffer == NULL) 
            return NULL;

        memset(buffer, 0, SECTOR_SIZE * sector_count);
        for (uint32_t i = 0; i < sector_count; i++) {
            uint8_t* sector_data = ATA_read_sector(lba + i);
            if (sector_data == NULL) return NULL;
            
            memcpy(buffer + i * SECTOR_SIZE, sector_data, SECTOR_SIZE);
            free(sector_data);
        }

        return buffer;
    }
//
//  READ SECTOR FROM DISK BY LBA ADRESS
//
///////////////////////////////////////////
//
//  WRITE DATA TO SECTOR ON DISK BY LBA ADRESS
//
    int ATA_write_sector(uint32_t lba, const uint8_t* buffer) {
        if (lba == BOOT_SECTOR) return -1;

        ATA_ata_wait();
        i386_outb(DRIVE_REGISTER, 0xE0 | ((lba >> 24) & 0x0F));
        i386_outb(FEATURES_REGISTER, 0x00);
        i386_outb(SECTOR_COUNT_REGISTER, 1);
        i386_outb(LBA_ADRESS_REGISTER, (uint8_t)lba);
        i386_outb(CYLINDER_LOW_REGISTER, (uint8_t)(lba >> 8));
        i386_outb(CYLINDER_HIGH_REGISTER, (uint8_t)(lba >> 16));
        i386_outb(STATUS_REGISTER, ATA_CMD_WRITE_PIO);

        int timeout = 9000000;
        while ((i386_inb(STATUS_REGISTER) & ATA_SR_BSY) == 0) 
            if (--timeout < 0) return -1;
            else continue;
        
        for (int i = 0; i < SECTOR_SIZE / 2; i++) {
            uint16_t data = *((uint16_t*)(buffer + i * 2));
            i386_outw(DATA_REGISTER, data);
        }

        return 1;
    }

    // Function to write a sector on the disk.
    char* ATA_write_sectors(uint32_t lba, const uint8_t* buffer, uint32_t sector_count) {
        ATA_ata_wait();
        for(uint32_t i = 0; i < sector_count; i++) {
            if (ATA_write_sector(lba + i, buffer) == -1) 
                return -1;
            
            buffer += SECTOR_SIZE;
        }

        return 1;
    }
//
//  WRITE DATA TO SECTOR ON DISK BY LBA ADRESS
//
///////////////////////////////////////////
//
//  OTHER FUNCTIONS
//
    // Function that add data to sector
    void ATA_append_sector(uint32_t lba, char* append_data) {
        char* previous_data = ATA_read_sector(lba);

        strcat(previous_data, append_data);
        ATA_write_sector(lba, previous_data);

        free(previous_data);
    }

    // Function that clear sector
    int ATA_clear_sector(uint32_t lba) {
        char buffer[SECTOR_SIZE];  // Assuming 512-byte sectors
        memset(buffer, 0, sizeof(buffer));

        // Write the buffer to the specified sector
        if (ATA_write_sector(lba, buffer) == -1) {
            kprintf("\n\rPulizia del settore non completata!");
            return -1;
        }

        return 1;
    }

    // Function to check if a sector (by LBA) is empty (all bytes are zero)
    bool ATA_is_current_sector_empty(uint32_t lba) {
        uint8_t* sector_data = ATA_read_sector(lba);
        if (ATA_is_sector_empty(sector_data)) {
            free(sector_data);
            return true;
        }

        free(sector_data);
        return false;
    }

    // Function to check if a sector is empty (contains all zeros).
    bool ATA_is_sector_empty(const uint8_t* sector_data) {
        for (int i = 0; i < SECTOR_SIZE; i++) 
            if (sector_data[i] != 0x00) 
                return false;
            
        return true;
    }

    // Function that find first empty sector
    uint32_t ATA_find_empty_sector(uint32_t offset) {
        for (uint32_t lba = offset; lba <= SECTOR_COUNT; lba++) {
            char* sector_data = ATA_read_sector(lba);
            if (sector_data == NULL) 
                continue;

            if (ATA_is_sector_empty((const uint8_t*)sector_data)) {
                free(sector_data);
                return lba;
            }
                
            free(sector_data);
        }

        return -1;
    }

    // Delay for working with ATA
    void ATA_ata_wait() {
        int delay = 150000;
        while (--delay > 0)
            continue;
    }

    // Function for getting avaliable sector count in disk
    int ATA_global_sector_count() {
        int sectors = 0;
        for (uint32_t lba = 0; lba < SECTOR_COUNT; lba++) {
            char* sector_data = ATA_read_sector(lba);
            if (sector_data != NULL) {
                sectors++;
                free(sector_data);
            }
        }

        return sectors;
    }

    // Function for getting empty sector count
    int ATA_global_sector_empty() {
        int sectors = 0;
        for (uint32_t lba = 0; lba < SECTOR_COUNT; lba++) {
            char* sector_data = ATA_read_sector(lba);
            if (sector_data != NULL) {
                if (ATA_is_sector_empty(sector_data))
                    sectors++;

                free(sector_data);
            }
        }

        return sectors;
    }
//
//  OTHER FUNCTIONS
//
///////////////////////////////////////////