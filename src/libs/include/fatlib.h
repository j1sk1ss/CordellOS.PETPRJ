#pragma once

#include "../include/stdlib.h"
#include "../include/stdio.h"

#define FILE_READ_ONLY      0x01
#define FILE_HIDDEN         0x02
#define FILE_SYSTEM         0x04
#define FILE_VOLUME_ID      0x08
#define FILE_DIRECTORY      0x10
#define FILE_ARCHIVE        0x20

#define FILE_LAST_LONG_ENTRY    0x40
#define ENTRY_FREE              0xE5
#define ENTRY_END               0x00
#define ENTRY_JAPAN             0x05
#define LAST_LONG_ENTRY         0x40

#define LOWERCASE_ISSUE	        0x01
#define BAD_CHARACTER	        0x02
#define BAD_TERMINATION         0x04
#define NOT_CONVERTED_YET       0x08
#define TOO_MANY_DOTS           0x10

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

struct UFATDate* FATLIB_get_date(short data, int type);

void FATLIB_fatname2name(char* input, char* output);
char* FATLIB_name2fatname(char* input);

struct udirectory_entry* FATLIB_create_entry(const char* filename, const char* ext, int isDir, uint32_t firstCluster, uint32_t filesize);