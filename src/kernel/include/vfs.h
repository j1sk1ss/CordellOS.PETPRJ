#ifndef VFS_H_
#define VFS_H_

#include <stdlib.h>
#include <memory.h>
#include <fatlib.h>

#include "ata.h"
#include "fat.h"


#define FAT_FS      0
#define EXT2_FS     1


typedef char* (*file_read)(Content*);
typedef void (*file_read_offset)(Content*, uint8_t*, uint32_t, uint32_t);
typedef void (*file_write)(const char*, char*);
typedef Directory* (*open_dir)(const unsigned int, unsigned char, short);
typedef Content* (*get_object)(const char*);
typedef int (*object_exists)(const char*);
typedef int (*put_object)(const char*, Content*);
typedef int (*delete_object)(const char*, const char*);
typedef Content* (*create_object)(char*, short, char*);
typedef int (*object_execute)(char*, int, char**);
typedef int (*object_meta_change)(const char*, directory_entry_t*);


typedef struct vfs_node {
    char name[256];
    uint32_t fs_type;
    struct ata_dev* device;

    //===========
    // Functions
    //===========

        file_read           read;
        file_read_offset    readoff;
        file_write          write;
        open_dir            dir;
        get_object          getobj;
        object_exists       objexist;
        put_object          putobj;
        delete_object       delobj;
        create_object       crobj;
        object_execute      objexec;
        object_meta_change  objmetachg;

    //===========
    // Functions
    //===========

    struct vfs_node* next;

} vfs_node_t;


extern struct vfs_node* vfs_list;
extern struct vfs_node* current_vfs;


void VFS_initialize(struct ata_dev* dev, uint32_t fs_type);
void VFS_add_node(struct ata_dev* dev, uint32_t fs_type);
void VFS_switch_device(int index);


#endif