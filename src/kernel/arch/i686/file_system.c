#include "../../include/file_system.h"


struct TempDirectory* mainDirectory;
struct TempDirectory* currentDirectory;

////////////////////////////////////////////////
//
//  CREATES TEMP DIRECTORY <NAME> IN CURRENT DIRECTORY
//

    void create_temp_directory(char* name) {
        if (find_temp_directory(name) != NULL) {
            printf("Directory alredy exist.");
            return;
        }

        struct TempDirectory* newDirectory = malloc(sizeof(struct TempDirectory));
        memset(newDirectory, 0, sizeof(newDirectory));

        newDirectory->name = malloc(strlen(name));
        memset(newDirectory->name, 0, sizeof(newDirectory->name));
        memcpy(newDirectory->name, name, strlen(name));
        
        newDirectory->files         = NULL;
        newDirectory->next          = NULL;
        newDirectory->subDirectory  = NULL;

        if (currentDirectory == NULL)
            currentDirectory = newDirectory;
        else {
            newDirectory->upDirectory = currentDirectory;

            if (currentDirectory->subDirectory == NULL) 
                currentDirectory->subDirectory = newDirectory;
            else {
                struct TempDirectory* current = currentDirectory->subDirectory;
                while (current->next != NULL) 
                    current = current->next;

                current->next = newDirectory;
            }
        }  
    }

//
//  CREATES TEMP DIRECTORY <NAME> IN CURRENT DIRECTORY
//
////////////////////////////////////////////////
//
//  CREATES TEMP FILE WITH NAME <NAME> IN CURRENT DIRECTORY
//

    void create_temp_file(char* type, char* name, uint8_t* sector) {
        if (find_temp_file(name) != NULL) {
            printf("File alredy exist.");
            return;
        }

        struct TempFile* newFile = malloc(sizeof(struct TempFile));
        memset(newFile, 0, sizeof(newFile));

        newFile->name = malloc(strlen(name));
        memset(newFile->name, 0, sizeof(newFile->name));
        memcpy(newFile->name, name, strlen(name));

        newFile->fileType = malloc(strlen(type));
        memcpy(newFile->fileType, type, strlen(type));

        newFile->sector = sector;
        writeSector(sector, "\0");

        if (currentDirectory->files == NULL)
            currentDirectory->files = newFile;
        else {
            struct TempFile* current = currentDirectory->files;
            while (current->next != NULL) 
                current = current->next;
            
            current->next = newFile;
        }

        
    }

//
//  CREATES TEMP FILE WITH NAME <NAME> IN CURRENT DIRECTORY
//
////////////////////////////////////////////////
//
//  DELETE TEMP DIRECTORY (EMPTY DIRECTORY)
//

    void delete_temp_directory(char* name) {
        struct TempDirectory* current   = currentDirectory->subDirectory;
        struct TempDirectory* prev      = NULL;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {

                if (current->files != NULL || current->subDirectory != NULL) {
                    printf("\r\nDirectory not empty. Use cordell.\r\n");
                    return;
                }

                if (prev == NULL) 
                    currentDirectory->subDirectory = current->next;
                else 
                    prev->next = current->next;
                
                free(current->name);
                free(current);

                

                break;
            }

            prev    = current;
            current = current->next;
        }
    }

//
//  DELETE TEMP DIRECTORY (EMPTY DIRECTORY)
////
//  DELETE TEMP DIRECTORY (ANY DIRECTORY)
//

    void cordell_delete_temp_directory(char* name) {
        struct TempDirectory* current   = currentDirectory->subDirectory;
        struct TempDirectory* prev      = NULL;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {
                if (prev == NULL) 
                    currentDirectory->subDirectory = current->next;
                else 
                    prev->next = current->next;
                
                free(current->name);
                free(current);

                

                break;
            }

            prev    = current;
            current = current->next;
        }
    }

//
//  DELETE TEMP DIRECTORY (ANY DIRECTORY)
//
////////////////////////////////////////////////
//
//  DELETE TEMP FILE (FILE WITH ACCESS LARGER THAN 0)
//

    void delete_temp_file(char* name) {
        struct TempFile* current    = currentDirectory->files;
        struct TempFile* prev       = NULL;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {

                if (strcmp(current->fileType, "0") == 0) {
                    printf("\r\nI don`t have permission. Use cordell.\r\n");
                    return;
                }

                clearSector(current->sector);

                if (prev == NULL) 
                    currentDirectory->files = current->next;
                else 
                    prev->next = current->next;
                
                free(current);
                
                break;
            }

            prev    = current;
            current = current->next;
        }
    }

//
//  DELETE TEMP FILE (FILE WITH ACCESS LARGER THAN 0)
////
//  DELETE TEMP FILE (ANY FILE)
//

    void cordell_delete_temp_file(char* name) {
        struct TempFile* current    = currentDirectory->files;
        struct TempFile* prev       = NULL;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {

                clearSector(current->sector);

                if (prev == NULL) 
                    currentDirectory->files = current->next;
                else 
                    prev->next = current->next;
                
                free(current);
                
                break;
            }

            prev    = current;
            current = current->next;
        }
    }

//
//  DELETE TEMP FILE (ANY FILE)
//
////////////////////////////////////////////////
//
//  OTHER FUNCTIONS
//

    struct TempFile* find_temp_file(char* name) {
        struct TempFile* current = currentDirectory->files;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) 
                return current;
            
            current = current->next;
        }

        return NULL;
    }

    struct TempDirectory* find_temp_directory(char* name) {
        struct TempDirectory* current = currentDirectory->subDirectory;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) 
                return current;

            current = current->next;
        }

        return NULL;
    }

    void move_to_temp_directory(char* name) {
        struct TempDirectory* neededDirectory = find_temp_directory(name);
        if (neededDirectory != NULL) 
            currentDirectory = neededDirectory;
    }

    void up_from_temp_directory() {
        if (currentDirectory->upDirectory != NULL)
            currentDirectory = currentDirectory->upDirectory;
    }

    struct TempDirectory* get_current_directory() {
        return currentDirectory;
    }

    char* get_full_temp_name() {
        char* name = malloc(0);
        memset(name, 0, sizeof(name));

        struct TempDirectory* current = currentDirectory;

        while (current != NULL) {
            char* temp_name = malloc(sizeof(name) + sizeof(current->name) + 1);
            memset(temp_name, 0, sizeof(temp_name));
            if (temp_name == NULL) {
                printf("[TFS 40] Alloc error!");
                return NULL;
            }

            strcpy(temp_name, name);

            temp_name = strcat(temp_name, current->name);
            temp_name = strcat(temp_name, "/");

            name    = temp_name;
            current = current->upDirectory;
        }

        return name;
    }

    void set_main_directory(struct TempDirectory* directory) {
        mainDirectory    = directory;
        currentDirectory = directory;
    }

    struct TempDirectory* get_main_directory() {
        return mainDirectory;
    }

//
//  OTHER FUNCTIONS
//
////////////////////////////////////////////////
//
//  SAVING DATA TO DISK
//

    void saveTempDirectory(struct TempDirectory* directory, char* result) {
        if (directory == NULL) 
            return;

        strcat(result, "D");
        strcat(result, directory->name);

        if (directory->subDirectory != NULL) {
            strcat(result, "N");
            saveTempDirectory(directory->subDirectory, result);
        }

        struct TempFile* file = directory->files;
        while (file != NULL) {
            strcat(result, "NF");
            strcat(result, file->name);
            strcat(result, "T");
            strcat(result, file->fileType);
            strcat(result, "S");

            char* sector_position = fprintf_unsigned(-1, file->sector, 10);
            strcat(result, sector_position);

            file = file->next;
            if (file != NULL) 
                strcat(result, "#");
        }

        strcat(result, "@");

        if (directory->next != NULL) {
            strcat(result, "#");
            saveTempDirectory(directory->next, result);
        }
    }

    struct TempFile* loadTempFile(const char* input, int* index) {
        struct TempFile* file = (struct TempFile*)malloc(sizeof(struct TempFile));
        file->fileType = NULL;
        file->name = NULL;
        
        if (input[*index] == 'F') {
            (*index)++; // Move past 'F'
            
            int start = *index;
            while (input[*index] != '\0' && input[*index] != 'T') 
                (*index)++;

            int length = *index - start;
            if (length > 0) {
                file->name = (char*)malloc(length + 1);
                strncpy(file->name, input + start, length);
                file->name[length] = '\0';
            }
            
            if (input[*index] == 'T') {
                (*index)++; // Move past 'T'
                
                start = *index;
                while (input[*index] != '\0' && input[*index] != 'S') 
                    (*index)++;

                length = *index - start;
                if (length > 0) {
                    file->fileType = (char*)malloc(length + 1);
                    strncpy(file->fileType, input + start, length);
                    file->fileType[length] = '\0';
                }
                
                if (input[*index] == 'S') {
                    (*index)++; // Move past 'S'
                    
                    start = *index;
                    while (input[*index] != '\0' && (input[*index] >= '0' && input[*index] <= '9')) {
                        (*index)++;
                    }

                    length = *index - start;
                    if (length > 0) {
                        char sectorStr[4];
                        strncpy(sectorStr, input + start, length);
                        sectorStr[length] = '\0';
                        int sector = atoi(sectorStr);
                        file->sector = (uint8_t)sector;
                    }
                }
            }
        }
        
        return file;
    }

    struct TempDirectory* loadTempDirectory(const char* input, int* index) {
        struct TempDirectory* directory = (struct TempDirectory*)malloc(sizeof(struct TempDirectory));

        directory->name         = NULL;
        directory->files        = NULL;
        directory->subDirectory = NULL;
        directory->upDirectory  = NULL;
        directory->next         = NULL;

        if (input[*index] == 'D') {
            (*index)++;
            int start = *index;
            while (input[*index] != '\0' && input[*index] != '@' && input[*index] != '#' && input[*index] != 'N') 
                (*index)++;

            char dirName[256];
            strncpy(dirName, input + start, *index - start);
            dirName[*index - start] = '\0';
            directory->name = strdup(dirName);
        }

        while (input[*index] == 'N') {
            (*index)++;

            if (input[*index] == 'D') {
                directory->subDirectory = loadTempDirectory(input, index);
                directory->subDirectory->upDirectory = directory;
            }

            if (input[*index] == 'F') 
                directory->files = loadTempFile(input, index);
        }

        while (input[*index] == '#') {
            (*index)++;
            if (input[*index] == 'D') 
                directory = loadTempDirectory(input, index);

            if (input[*index] == 'F') {
                (*index)++;
                struct TempFile* file = (struct TempFile*)malloc(sizeof(struct TempFile));
                file->fileType = NULL;
                file->name = NULL;

                int start = *index;
                while (input[*index] != '\0' && input[*index] != '#' && input[*index] != '@') 
                    (*index)++;

                if (input[start] == 'T') {
                    start++;
                    file->fileType = (char*)malloc(2);
                    strncpy(file->fileType, input + start, 1);
                    file->fileType[1] = '\0';
                }

                start = *index;
                while (input[*index] != '\0' && input[*index] != '#' && input[*index] != '@') 
                    (*index)++;

                if (input[start] == 'T') {
                    start++;
                    file->name = (char*)malloc(2);
                    strncpy(file->name, input + start, 1);
                    file->name[1] = '\0';
                }

                start = *index;
                while (input[*index] != '\0' && input[*index] != '#' && input[*index] != '@') 
                    (*index)++;

                if (input[start] == 'S') {
                    start++;
                    int sector = atoi(input + start);
                    file->sector = (uint8_t)sector;
                }

                struct TempFile* end_file = directory->files;
                while (end_file->next != NULL)
                    end_file = end_file->next;

                end_file->next = file;
            }
        }

        if (input[*index] == '@') 
            (*index)++;

        return directory;
    }

//
//  SAVING DATA TO DISK
//
////////////////////////////////////////////////

void init_directory() {
    currentDirectory = NULL;

    create_temp_directory("root");

    mainDirectory = currentDirectory;
}