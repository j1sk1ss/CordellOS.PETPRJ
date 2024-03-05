#ifndef FATLIB_H_
#define FATLIB_H_

#include "stdlib.h"
#include "stdio.h"
#include "stdlib.h"


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

typedef struct UFATFile {
	udirectory_entry_t file_meta;
	void* data_pointer;

	int data_size;
	uint32_t* data;

	char* extension;
	char* name;

    UFile* next;
} UFile;

typedef struct UFATDirectory {
	udirectory_entry_t directory_meta;
	void* data_pointer;

	char* name;

	UDirectory* next;
    UFile* files;
    UDirectory* subDirectory;
} UDirectory;

typedef struct UFATDate {
	uint16_t hour;
	uint16_t minute;
	uint16_t second;

	uint16_t year;
	uint16_t mounth;
	uint16_t day;
} UDate;

typedef struct UFATContent {
	UDirectory* directory;
	UFile* file;
} UContent;


void FATLIB_unload_directories_system(UDirectory* directory);
void FATLIB_unload_files_system(UFile* file);
void FATLIB_unload_content_system(UContent* content);

char* FATLIB_change_path(const char* currentPath, const char* content);

UDate* FATLIB_get_date(short data, int type);

void FATLIB_fatname2name(char* input, char* output);
char* FATLIB_name2fatname(char* input);

udirectory_entry_t* FATLIB_create_entry(const char* filename, const char* ext, int isDir, uint32_t firstCluster, uint32_t filesize);

#endif