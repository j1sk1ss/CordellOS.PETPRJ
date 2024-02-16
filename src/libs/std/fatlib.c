#include "../include/fatlib.h"

void FATLIB_unload_directories_system(struct UFATDirectory* directory) {
    if (directory == NULL) return;
    if (directory->files != NULL) FATLIB_unload_files_system(directory->files);
    if (directory->subDirectory != NULL) FATLIB_unload_directories_system(directory->subDirectory);
    if (directory->next != NULL) FATLIB_unload_directories_system(directory->next);
    if (directory->data_pointer != NULL) free(directory->data_pointer);

    free(directory->name);
    free(directory);
}

void FATLIB_unload_files_system(struct UFATFile* file) {
    if (file == NULL) return;
    if (file->next != NULL) FATLIB_unload_files_system(file->next);
    if (file->data_pointer != NULL) free(file->data_pointer);
    if (file->data != NULL) free(file->data);

    free(file->name);
    free(file->extension);
    free(file);
}

struct UFATDate* FATLIB_get_date(short data, int type) {
    struct UFATDate* date = malloc(sizeof(struct UFATDate));
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

    free(date);
    return NULL;
}

// Return NULL if can`t make updir command
char* FATLIB_change_path(const char* currentPath, const char* content) {
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

void FATLIB_fatname2name(char* input, char* output) {
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

char* FATLIB_name2fatname(char* input) {
    str_uppercase(input);

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

unsigned short FATLIB_current_time() {
    short data[7];
    get_datetime(&data);
    return (data[2] << 11) | (data[1] << 5) | (data[0] / 2);
}

	//clock nor date has been implemented yet
unsigned short FATLIB_current_date() {
    short data[7];
    get_datetime(&data);

    uint16_t reversed_data = 0;

    reversed_data |= data[3] & 0x1F;
    reversed_data |= (data[4] & 0xF) << 5;
    reversed_data |= ((data[5] - 1980) & 0x7F) << 9;

    return reversed_data;
}

//clock hasn't been implemented yet
unsigned char FATLIB_current_time_temths() {
    short data[7];
    get_datetime(&data);
    return (data[2] << 11) | (data[1] << 5) | (data[0] / 2);
}

int FATLIB_name_check(char * input) {
    short retVal = 0;

    //Invalid Values:
    /*Values less than 0x20 except for the special case of 0x05 in DIR_Name[0] described above.
         0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D,
        and 0x7C.*/
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

struct udirectory_entry* FATLIB_create_entry(const char* filename, const char* ext, int isDir, uint32_t firstCluster, uint32_t filesize) {
    struct udirectory_entry* data = malloc(sizeof(struct udirectory_entry));

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

    data->creation_date 		 = FATLIB_current_date();
    data->creation_time 		 = FATLIB_current_time();
    data->creation_time_tenths 	 = FATLIB_current_time_temths();

    if (FATLIB_name_check(file_name) != 0)
        FATLIB_name2fatname(file_name);

    strncpy(data->file_name, file_name, min(11, strlen(file_name)));
    return data; 
}