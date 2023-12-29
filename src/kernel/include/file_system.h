#pragma once

#include <stdint.h>

#include "../../libs/core/shared/include/math.h"

#include "string.h"
#include "ata.h"
#include "memory.h"
#include "stdio.h"


#define GRUB_OFFSET                 10000

#define FILE_SYSTEM_SECTOR          314 + GRUB_OFFSET
#define USERS_SECTOR                315 + GRUB_OFFSET
#define GROUPS_SECTOR               316 + GRUB_OFFSET
#define SHELL_SECTOR                317 + GRUB_OFFSET

#define FILES_SECTOR_OFFSET         450 + GRUB_OFFSET
#define SYS_FILES_SECTOR_OFFSET     320 + GRUB_OFFSET


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

struct Directory* get_current_directory();

char* get_full_temp_name();

void init_directory();
void create_directory(char* name);
void create_file(int read, int write, int edit, char* name, char* extension, uint8_t* head_sector);
void delete_directory(char* name);
void delete_file(char* name);

void write_file(struct File* file, char* data);
char* read_file(struct File* file);
void clear_file(struct File* file);
int file_exist(char* name);

struct File* find_file(char* name);
struct Directory* find_directory(char* name);

void move_to_directory(char* name);
void up_from_directory();

void set_main_directory(struct Directory* directory);
struct Directory* get_main_directory();

char* save_directory(struct Directory* directory);
struct Directory* load_directory(const char* input, int* index);
void save_file_system();