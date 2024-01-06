#pragma once

#include <stdint.h>

#include "../../libs/include/math.h"

#include "string.h"
#include "ata.h"
#include "memory.h"
#include "stdio.h"


#define GRUB_OFFSET                 12000

#define FILE_SYSTEM_SECTOR          350 + GRUB_OFFSET

#define FILES_SECTOR_OFFSET         450 + GRUB_OFFSET
#define SYS_FILES_SECTOR_OFFSET     354 + GRUB_OFFSET


struct File {
    int read_level;
    int write_level;
    int edit_level;

    char name[11];
    char extension[3];

    uint32_t* sectors;
    size_t sector_count;
    
    struct File* next;
};

struct Directory  {
    char name[11];

    struct File* files;

    struct Directory* next;

    struct Directory* subDirectory;
    struct Directory* upDirectory;
};

struct Directory* FS_get_current_directory();

char* FS_get_full_temp_name();

void FS_init();

void FS_create_directory(char* name);
void FS_create_file(int read, int write, int edit, char* name, char* extension, uint8_t* head_sector);

void FS_delete_directory(char* name, struct Directory* directory);
void FS_delete_file(char* name, struct Directory* directory);

void FS_write_file(struct File* file, char* data);
char* FS_read_file(struct File* file);
void FS_clear_file(struct File* file);

int FS_file_exist(char* name);

struct File* FS_global_find_file(char* path);
struct Directory* FS_global_find_directory(char* path);

int FS_move_to_directory(char* name, struct Directory* directory);
void FS_up_from_directory();

void FS_set_main_directory(struct Directory* directory);
struct Directory* FS_get_main_directory();

char* FS_save_directory(struct Directory* directory);
struct Directory* FS_load_directory(const char* input, int* index);
void FS_save_file_system();