#include "../../include/disk.h"

///////////////////////////////////////////
//
//  FAT32 DISK
//

    // Function to convert Logical Block Address (LBA) to Cylinder-Head-Sector (CHS) format
    void DISK_lba_to_chs(DISK* disk, uint32_t lba, uint16_t* cylinderOut, uint16_t* sectorOut, uint16_t* headOut) {
        *sectorOut = lba % disk->sectors + 1;
        *cylinderOut = (lba / disk->sectors) / disk->heads;
        *headOut = (lba / disk->sectors) % disk->heads;
    }

    // Function to read sectors from the disk
    bool DISK_read_sectors(DISK* disk, uint32_t lba, uint8_t sectors, void* dataOut) {
        uint16_t cylinder, sector, head;

        DISK_lba_to_chs(disk, lba, &cylinder, &sector, &head);

        for (int i = 0; i < 3; i++) {
            if (x86_disk_read(disk->id, cylinder, sector, head, sectors, dataOut))
                return true;

            x86_disk_reset(disk->id);
        }

        return false;
    }

//
//  FAT32 DISK
//
///////////////////////////////////////////