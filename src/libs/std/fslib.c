#include "../include/fslib.h"


void FSLIB_unload_directories_system(Directory* directory) {
    if (directory == NULL) return;
    if (directory->files != NULL) FSLIB_unload_files_system(directory->files);
    if (directory->subDirectory != NULL) FSLIB_unload_directories_system(directory->subDirectory);
    if (directory->next != NULL) FSLIB_unload_directories_system(directory->next);
    if (directory->data_pointer != NULL) free(directory->data_pointer);

    free(directory->name);
    free(directory);
}

void FSLIB_unload_files_system(File* file) {
    if (file == NULL) return;
    if (file->next != NULL) FSLIB_unload_files_system(file->next);
    if (file->data_pointer != NULL) free(file->data_pointer);
    if (file->data != NULL) free(file->data);

    free(file->name);
    free(file->extension);
    free(file);
}

void FSLIB_unload_content_system(Content* content) {
    if (content == NULL) return;
    if (content->directory != NULL) FSLIB_unload_directories_system(content->directory);
    if (content->file != NULL) FSLIB_unload_files_system(content->file);
    
    free(content);
}

// 1 - date
// 2 - time
Date* FSLIB_get_date(short data, int type) {
    Date* date = malloc(sizeof(Date));
    switch (type) {
        case 1: // date
            date->year   = ((data >> 9) & 0x7F) + 1980;
            date->mounth = (data >> 5) & 0xF;
            date->day    = data & 0x1F;
            return date;

        break;

        case 2: // time
            date->hour   = (data >> 11) & 0x1F;
            date->minute = (data >> 5) & 0x3F;
            date->second = (data & 0x1F) * 2;
            return date;

        break;
    }

    return date;
}

// Return NULL if can`t make updir command
char* FSLIB_change_path(const char* currentPath, const char* content) {
    if (content == NULL || content[0] == '\0') {
        const char* lastSeparator = strrchr(currentPath, '\\');
        if (lastSeparator == NULL) return NULL;
        else {
            size_t parentPathLen = lastSeparator - currentPath;
            char* parentPath = malloc(parentPathLen + 1);
            if (parentPath == NULL) {
                printf("Memory allocation failed\n");
                return NULL;
            }

            strncpy(parentPath, currentPath, parentPathLen);
            parentPath[parentPathLen] = '\0';

            return parentPath;
        }
    }
    
    else {
        int newPathLen = strlen(currentPath) + strlen(content) + 2;
        char* newPath  = malloc(newPathLen);
        if (newPath == NULL) return NULL;

        strcpy(newPath, currentPath);
        if (newPath[strlen(newPath) - 1] != '\\') 
            strcat(newPath, "\\");

        strcat(newPath, content);
        newPath[newPathLen - 1] = '\0';

        return newPath;
    }
}

void FSLIB_fatname2name(char* input, char* output) {
    if (input[0] == '.') {
        if (input[1] == '.') {
            strcpy (output, "..");
            return;
        }

        strcpy (output, ".");
        return;
    }

    unsigned short counter = 0;
    for ( counter = 0; counter < 8; counter++) {
        if (input[counter] == 0x20) {
            output[counter] = '.';
            break;
        }

        output[counter] = input[counter];
    }

    if (counter == 8) 
        output[counter] = '.';

    unsigned short counter2 = 8;
    for (counter2 = 8; counter2 < 11; counter2++) {
        ++counter;
        if (input[counter2] == 0x20 || input[counter2] == 0x20) {
            if (counter2 == 8)
                counter -= 2;

            break;
        }
        
        output[counter] = input[counter2];		
    }

    ++counter;
    while (counter < 12) {
        output[counter] = ' ';
        ++counter;
    }

    output[12] = '\0';
    return;
}

char* FSLIB_name2fatname(char* input) {
    str2uppercase(input);

    int haveExt = 0;
    char searchName[13] = { '\0' };
    unsigned short dotPos = 0;

    unsigned int counter = 0;
    while (counter <= 8) {
        if (input[counter] == '.' || input[counter] == '\0') {
            if (input[counter] == '.') haveExt = 1;

            dotPos = counter;
            counter++;

            break;
        }
        else {
            searchName[counter] = input[counter];
            counter++;
        }
    }

    if (counter > 9) {
        counter = 8;
        dotPos = 8;
    }
    
    unsigned short extCount = 8;
    while (extCount < 11) {
        if (input[counter] != '\0' && haveExt == 1) searchName[extCount] = input[counter];
        else searchName[extCount] = ' ';

        counter++;
        extCount++;
    }

    counter = dotPos;
    while (counter < 8) {
        searchName[counter] = ' ';
        counter++;
    }

    strcpy(input, searchName);
    return input;
}

unsigned short FSLIB_current_time() {
    short data[7];
    get_datetime(data);
    return (data[2] << 11) | (data[1] << 5) | (data[0] / 2);
}

unsigned short FSLIB_current_date() {
    short data[7];
    get_datetime(data);

    uint16_t reversed_data = 0;

    reversed_data |= data[3] & 0x1F;
    reversed_data |= (data[4] & 0xF) << 5;
    reversed_data |= ((data[5] - 1980) & 0x7F) << 9;

    return reversed_data;
}

unsigned char FSLIB_current_time_temths() {
    short data[7];
    get_datetime(data);
    return (data[2] << 11) | (data[1] << 5) | (data[0] / 2);
}

int FSLIB_name_check(char* input) {
    short retVal = 0;
    unsigned short iterator;
    for (iterator = 0; iterator < 11; iterator++) {
        if (input[iterator] < 0x20 && input[iterator] != 0x05) {
            retVal = retVal | BAD_CHARACTER;
        }
        
        switch (input[iterator]) {
            case 0x2E: {
                if ((retVal & NOT_CONVERTED_YET) == NOT_CONVERTED_YET) //a previous dot has already triggered this case
                    retVal |= TOO_MANY_DOTS;

                retVal ^= NOT_CONVERTED_YET; //remove NOT_CONVERTED_YET flag if already set

                break;
            }

            case 0x22:
            case 0x2A:
            case 0x2B:
            case 0x2C:
            case 0x2F:
            case 0x3A:
            case 0x3B:
            case 0x3C:
            case 0x3D:
            case 0x3E:
            case 0x3F:
            case 0x5B:
            case 0x5C:
            case 0x5D:
            case 0x7C:
                retVal = retVal | BAD_CHARACTER;
        }

        if (input[iterator] >= 'a' && input[iterator] <= 'z') 
            retVal = retVal | LOWERCASE_ISSUE;
    }

    return retVal;
}

unsigned char FSLIB_check_sum(unsigned char *pFcbName) {
    short FcbNameLen;
    unsigned char Sum;
    Sum = 0;
    for (FcbNameLen = 11; FcbNameLen != 0; FcbNameLen--) 
        Sum = ((Sum & 1) ? 0x80 : 0) + (Sum >> 1) + *pFcbName++;

    return (Sum);
}

directory_entry_t* FSLIB_create_entry(const char* filename, const char* ext, int isDir, uint32_t firstCluster, uint32_t filesize) {
    directory_entry_t* data = malloc(sizeof(directory_entry_t));

    data->reserved0 				= 0; 
    data->creation_time_tenths 		= 0;
    data->creation_time 			= 0;
    data->creation_date 			= 0;
    data->last_modification_date 	= 0;

    char* file_name = (char*)malloc(25);
    strcpy(file_name, filename);
    if (ext) {
        strcat(file_name, ".");
        strcat(file_name, ext);
    }
    
    data->low_bits 	= firstCluster;
    data->high_bits = firstCluster >> 16;  

    if(isDir == 1) {
        data->file_size 	= 0;
        data->attributes 	= FILE_DIRECTORY;
    } else {
        data->file_size 	= filesize;
        data->attributes 	= FILE_ARCHIVE;
    }

    data->creation_date 	   = FSLIB_current_date();
    data->creation_time 	   = FSLIB_current_time();
    data->creation_time_tenths = FSLIB_current_time_temths();

    if (FSLIB_name_check(file_name) != 0)
        FSLIB_name2fatname(file_name);

    strncpy(data->file_name, file_name, min(11, strlen(file_name)));
    return data; 
}

Content* FSLIB_create_content(char* name, short directory, char* extension) {
    Content* content = malloc(sizeof(Content));
    if (strlen(name) > 11 || strlen(extension) > 4) {
        printf("Uncorrect name or ext lenght.\n");
        return NULL;
    }
    
    if (directory == 1) {
        content->directory = malloc(sizeof(Directory));
        strncpy(content->directory->name, name, 11);
        content->directory->directory_meta = *FSLIB_create_entry(name, NULL, 1, NULL, NULL);
    }
    else {
        content->file = malloc(sizeof(File));
        strncpy(content->file->name, name, 11);
        strncpy(content->file->extension, extension, 4);
        content->file->file_meta = *FSLIB_create_entry(name, extension, 0, NULL, 1);
    }

    return content;
}

//====================================================================
//  Read file content by path
//  ECX - path
//  EAX - returned data
char* fread(const char* path) {
    void* pointed_data;
    __asm__ volatile(
        "movl $9, %%eax\n"
        "movl %1, %%ebx\n"
        "movl $0, %%ecx\n"
        "int %2\n"
        "movl %%eax, %0\n"
        : "=r"(pointed_data)
        : "r"((uint32_t)path), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );

    return pointed_data;
}

//====================================================================
// Function get pixel from framebuffer by coordinates
// EBX - content pointer
// ECX - data offset
// EDX - buffer pointer
// ESI - buffer len / data len
void fread_off(Content* content, int offset, uint8_t* buffer, int len) {
    __asm__ volatile(
        "movl $33, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "movl %2, %%edx\n"
        "movl %3, %%esi\n"
        "int $0x80\n"
        :
        : "g"(content), "g"(offset), "g"(buffer), "g"(len)
        : "eax", "ebx", "ecx", "edx", "esi"
    );
}

//====================================================================
//  Write file to content (if it exists) by path (rewrite all content)
//  EBX - path
//  ECX - data
void fwrite(const char* path, const char* data) {
    __asm__ volatile(
        "movl $10, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "int $0x80\n"
        :
        : "r"((uint32_t)path), "r"((uint32_t)data)
        : "eax", "ebx", "ecx"
    );
}

//====================================================================
//  Returns linked list of dir content by path
//  EBX - path
//  ECX - pointer to directory
Directory* opendir(const char* path) {
    Directory* directory;
    __asm__ volatile(
        "movl $11, %%eax\n"
        "movl %1, %%ebx\n"
        "int $0x80\n"
        "movl %%eax, %0\n"
        : "=r"(directory)
        : "r"((uint32_t)path)
        : "eax", "ebx", "ecx"
    );

    return directory;
}

//====================================================================
//  Returns linked list of dir content by path
//  EBX - path
//  ECX - pointer to directory
Content* get_content(const char* path) {
    Content* content;
    __asm__ volatile(
        "movl $30, %%eax\n"
        "movl %1, %%ebx\n"
        "movl %2, %%ecx\n"
        "int $0x80\n"
        "movl %%eax, %0\n"
        : "=r"(content)
        : "r"((uint32_t)path), "r"(content)
        : "eax", "ebx", "ecx"
    );

    return content;
}

//====================================================================
//  Function for checking if content exist by this path
//  EBX - path
//  ECX - result (0 - nexists)
int cexists(const char* path) {
    int result;

    __asm__ volatile(
        "movl $15, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "int $0x80\n"
        : 
        : "r"((uint32_t)path), "r"(&result)
        : "eax", "ebx"
    );

    return result;
}

//====================================================================
//  This function creates file
//  EBX - path
//  RCX - name (with extention)
void mkfile(const char* path, const char* name) {
    __asm__ volatile(
        "movl $16, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "int $0x80\n"
        :
        : "r"((uint32_t)path), "r"((uint32_t)name)
        : "eax", "ebx", "ecx"
    );
}

//====================================================================
//  This function creates directory
//  EBX - path
//  ECX - name
void mkdir(const char* path, const char* name) {
    __asm__ volatile(
        "movl $17, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "int $0x80\n"
        :
        : "r"((uint32_t)path), "r"((uint32_t)name)
        : "eax", "ebx", "ecx"
    );
}

//====================================================================
//  This function remove content
//  EBX - path
//  ECX - name (if file - with extention)
void rmcontent(const char* path, const char* name) {
    __asm__ volatile(
        "movl $18, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "int $0x80\n"
        :
        : "r"((uint32_t)path), "r"((uint32_t)name)
        : "eax", "ebx", "ecx"
    );
}

//====================================================================
// This function change content meta by path
// EBX - path
// ECX - new meta
void chgcontent(const char* path, directory_entry_t* meta) {
    __asm__ volatile(
        "movl $25, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "int $0x80\n"
        :
        : "r"((uint32_t)path), "r"((uint32_t)meta)
        : "eax", "ebx", "ecx"
    );
}

//====================================================================
//  Function that executes ELF file
//  EAX - result CODE
//  EBX - path
//  ECX - args (count)
//  EDX - argv (array of args)
int fexec(char* path, int argc, char** argv) {
    int result = 0;
    __asm__ volatile(
        "movl $12, %%eax\n"
        "movl %1, %%ebx\n"
        "movl %2, %%ecx\n"
        "movl %3, %%edx\n"
        "int $0x80\n"
        "movl %%eax, %0\n"
        : "=r"(result)
        : "m"((uint32_t)path), "r"(argc), "m"(argv)
        : "eax", "ebx", "ecx", "edx"
    );

    return result;
}