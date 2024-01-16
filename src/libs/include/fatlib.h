#pragma once

#include "../include/syscall.h"
#include "../include/stdio.h"

typedef struct udirectory_entry {
	unsigned char file_name[11];
	unsigned char attributes;
	unsigned char reserved0;
	unsigned char creation_time_tenths;
	unsigned short creation_time;
	unsigned short creation_date;
	unsigned short last_accessed;
	unsigned short high_bits;
	unsigned short last_modification_time;
	unsigned short last_modification_date;
	unsigned short low_bits;
	unsigned int file_size;
} __attribute__((packed)) udirectory_entry_t;

struct UFATFile {
	udirectory_entry_t file_meta;

	int data_size;
	uint32_t* data;

	char extension[4];
	char name[11];

    struct File* next;
};

struct UFATDirectory {
	udirectory_entry_t directory_meta;

	char name[11];

	struct UFATDirectory* next;

    struct UFATFile* files;
    struct UFATDirectory* subDirectory;
};

struct UFATDate {
	uint16_t hour;
	uint16_t minute;
	uint16_t second;

	uint16_t year;
	uint16_t mounth;
	uint16_t day;
};

void FATLIB_unload_directories_system(struct UFATDirectory* directory);
void FATLIB_unload_files_system(struct UFATFile* file);

char* FATLIB_change_path(const char* currentPath, const char* content);