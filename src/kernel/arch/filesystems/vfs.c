#include "../../include/vfs.h"


struct vfs_node* vfs_list = NULL;
struct vfs_node* current_vfs = NULL;


void VFS_initialize(struct ata_dev* dev, uint32_t fs_type) {
    vfs_list = malloc(sizeof(struct vfs_node));
    vfs_list->fs_type = fs_type;
    vfs_list->device  = dev;

    if (fs_type == FAT_FS) {
        vfs_list->read       = FAT_read_content;
        vfs_list->readoff    = FAT_read_content2buffer;
        vfs_list->write      = FAT_edit_content;
        vfs_list->dir        = FAT_directory_list;
        vfs_list->getobj     = FAT_get_content;
        vfs_list->objexist   = FAT_content_exists;
        vfs_list->putobj     = FAT_put_content;
        vfs_list->delobj     = FAT_delete_content;
        vfs_list->crobj      = FAT_create_content;
        vfs_list->objexec    = FAT_ELF_execute_content;
        vfs_list->objmetachg = FAT_change_meta;

        strncpy(vfs_list->name, "FATFS", 5);
    } 

    current_vfs = vfs_list;
}

void VFS_add_node(struct ata_dev* dev, uint32_t fs_type) {
    struct vfs_node* new_node = malloc(sizeof(struct vfs_node));
    new_node->fs_type = fs_type;
    new_node->device  = dev;

    if (fs_type == FAT_FS) {
        new_node->read       = FAT_read_content;
        new_node->readoff    = FAT_read_content2buffer;
        new_node->write      = FAT_edit_content;
        new_node->dir        = FAT_directory_list;
        new_node->getobj     = FAT_get_content;
        new_node->objexist   = FAT_content_exists;
        new_node->putobj     = FAT_put_content;
        new_node->delobj     = FAT_delete_content;
        new_node->crobj      = FAT_create_content;
        new_node->objexec    = FAT_ELF_execute_content;
        new_node->objmetachg = FAT_change_meta;

        strncpy(new_node->name, "FATFS", 5);
    }

    struct vfs_node* cur = vfs_list;
    while (cur->next != NULL) cur = cur->next;
    cur->next = new_node;
}

void VFS_switch_device(int index) {
    int pos = 0;
    while (current_vfs->next != NULL) {
        current_vfs = current_vfs->next;
        if (pos++ == index) break;
    }
    
    ATA_device_switch(index);
    if (current_vfs->fs_type == FAT_FS) FAT_initialize();
}