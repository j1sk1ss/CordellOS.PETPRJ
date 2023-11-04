#pragma once

#include <stdint.h>

#include "string.h"
#include "ata.h"
#include "memory.h"
#include "stdio.h"

#include "../shell/include/table.h"

struct File {
    char* fileType;

    char* name;
    uint32_t* sectors;
    size_t sector_count;
    
    struct File* next;
};

struct Directory  {
    char* name;

    struct File* files;

    struct Directory* next;

    struct Directory* subDirectory;
    struct Directory* upDirectory;
};

struct Directory* get_current_directory();

char* get_full_temp_name();

void print_directory_data();

void init_directory();
void create_temp_directory(char* name);
void create_temp_file(char* type, char* name, uint8_t* sector);
void delete_temp_directory(char* name);
void cordell_delete_temp_directory(char* name);
void delete_temp_file(char* name);
void cordell_delete_temp_file(char* name);

void write_temp_file(struct File* file, char* data);
char* read_temp_file(struct File* file);
void clear_temp_file(struct File* file);

struct File* find_temp_file(char* name);
struct Directory* find_temp_directory(char* name);

void move_to_temp_directory(char* name);
void up_from_temp_directory();

void set_main_directory(struct Directory* directory);
struct Directory* get_main_directory();

void save_temp_directory(struct Directory* directory, char* result);
struct Directory* load_temp_directory(const char* input, int* index);