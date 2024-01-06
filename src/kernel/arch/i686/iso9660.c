#include "../../include/iso9660.h"

void ISO_init() {
    printf("ISO inits\n");
    struct grub_iso9660_susp_entry* bootsect = (struct grub_iso9660_susp_entry*)ATA_read_sector(0, 1);

    printf("TYPE: [%i]\n", bootsect->sig);
    printf("Identifier: [%i]\n", bootsect->len);
    printf("VER: [%i]\n", bootsect->version);
    printf("SYS: [%i]\n", bootsect->data);
}