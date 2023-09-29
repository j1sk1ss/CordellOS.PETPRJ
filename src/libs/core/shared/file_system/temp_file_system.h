#pragma once

#include "../include/string.h"

struct File {
    char* fileType;

    char* name;
    char* content;

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

void init_directory();
void create_temp_directory(char* name);
void create_temp_file(char* type, char* name, char** content);
void delete_temp_directory(char* name);
void cordell_delete_temp_directory(char* name);
void delete_temp_file(char* name);
void cordell_delete_temp_file(char* name);

struct File* find_temp_file(char* name);
struct Directory* find_temp_directory(char* name);

void move_to_temp_directory(char* name);
void up_from_temp_directory();