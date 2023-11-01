#pragma once

#include <stdint.h>

#include "string.h"
#include "ata.h"
#include "memory.h"

struct TempFile {
    char* fileType;

    char* name;
    uint8_t sector;

    struct TempFile* next;
};

struct TempDirectory  {
    char* name;

    struct TempFile* files;

    struct TempDirectory* next;

    struct TempDirectory* subDirectory;
    struct TempDirectory* upDirectory;
};

struct TempDirectory* get_current_directory();

char* get_full_temp_name();

void init_directory();
void create_temp_directory(char* name);
void create_temp_file(char* type, char* name, uint8_t* sector);
void delete_temp_directory(char* name);
void cordell_delete_temp_directory(char* name);
void delete_temp_file(char* name);
void cordell_delete_temp_file(char* name);

struct TempFile* find_temp_file(char* name);
struct TempDirectory* find_temp_directory(char* name);

void move_to_temp_directory(char* name);
void up_from_temp_directory();

void set_main_directory(struct TempDirectory* directory);
struct TempDirectory* get_main_directory();

void saveTempDirectory(struct TempDirectory* directory, char* result, int* index);
struct TempDirectory* loadTempDirectory(const char* input, int* index);