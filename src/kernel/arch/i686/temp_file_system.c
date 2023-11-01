#include "../../include/temp_file_system.h"


struct TempDirectory* mainDirectory;
struct TempDirectory* currentDirectory;

void init_directory() {
    currentDirectory = NULL;
    create_temp_directory("root");

    mainDirectory = currentDirectory;
}

////////////////////////////////////////////////
//
//  CREATES TEMP DIRECTORY <NAME> IN CURRENT DIRECTORY
//

    void create_temp_directory(char* name) {
        struct TempDirectory* newDirectory = malloc(sizeof(struct TempDirectory));
        memset(newDirectory, 0, sizeof(newDirectory));

        newDirectory->name  = malloc(strlen(name));
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
        struct TempFile* newFile = malloc(sizeof(struct TempFile));
        memset(newFile, 0, sizeof(newFile));

        newFile->name = malloc(strlen(name));;
        memcpy(newFile->name, name, strlen(name));

        newFile->fileType   = type;
        newFile->sector     = sector;

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

        printf("\r\nFile not found.");
        return NULL;
    }

    struct TempDirectory* find_temp_directory(char* name) {
        struct TempDirectory* current = currentDirectory->subDirectory;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) 
                return current;

            current = current->next;
        }

        printf("\r\nDirectory not found.");
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

    void saveTempDirectory(struct TempDirectory* directory, char* result, int* index) {
        if (directory == NULL) {
            return;
        }

        // Add directory name to the result
        strcat(result, directory->name);
        (*index) += strlen(directory->name);

        // Add a separator to differentiate between directory and file names
        strcat(result, "|");
        (*index)++;

        // Recursively convert subdirectories
        saveTempDirectory(directory->subDirectory, result, index);

        // Add a separator to differentiate between subdirectories and files
        strcat(result, "#");
        (*index)++;

        // Recursively convert files
        struct TempFile* file = directory->files;
        while (file != NULL) {
            // Add file type, name, and sector information
            strcat(result, file->fileType);
            strcat(result, "|");
            strcat(result, file->name);
            strcat(result, "|");
            strcat(result, file->sector);

            (*index) += strlen(file->fileType) + strlen(file->name) + strlen(file->sector);

            if (file->next != NULL) {
                // Add a separator to differentiate between files
                strcat(result, ",");
                (*index)++;
            }

            file = file->next;
        }

        // Recursively convert next directories
        strcat(result, "@");
        (*index)++;

        saveTempDirectory(directory->next, result, index);
    }

    struct TempDirectory* loadTempDirectory(const char* input, int* index) {
        // Extract the directory name
        int start = *index;
        while (input[*index] != '|') {
            (*index)++;
        }
        
        char dirName[256];
        strncpy(dirName, input + start, *index - start);
        dirName[*index - start] = '\0';

        // Create a directory node
        struct TempDirectory* directory = malloc(sizeof(struct TempDirectory));
        directory->name = strdup(dirName);
        directory->upDirectory = NULL;

        // Move past the separator
        (*index)++;

        // Recursively create subdirectories
        if (input[*index] == '|') {
            (*index)++;
            directory->subDirectory = loadTempDirectory(input, index);
        } else {
            directory->subDirectory = NULL;
        }

        // Move past the separator
        (*index)++;

        // Initialize file list
        directory->files = NULL;
        struct TempFile* currentFile = NULL;

        // Parse files
        while (input[*index] != '@') {
            char fileType[4];
            start = *index;

            while (input[*index] != '|') {
                (*index)++;
            }

            strncpy(fileType, input + start, *index - start);
            fileType[*index - start] = '\0';

            // Move past the separator
            (*index)++;

            start = *index;
            while (input[*index] != '|') {
                (*index)++;
            }

            char fileName[256];
            strncpy(fileName, input + start, *index - start);
            fileName[*index - start] = '\0';

            // Move past the separator
            (*index)++;

            // Extract and store the sector directly as an integer
            char sectorStr[4];
            start = *index;
            while (input[*index] != ',' && input[*index] != '@') {
                sectorStr[*index - start] = input[*index];
                (*index)++;
            }

            sectorStr[*index - start] = '\0';
            int sector = atoi(sectorStr);

            // Create a file node
            struct TempFile* file = malloc(sizeof(struct TempFile));
            file->fileType = strdup(fileType);
            file->name = strdup(fileName);
            file->sector = sector;
            file->next = NULL;

            if (currentFile == NULL) 
                directory->files = file;
                currentFile = file;
            else 
                currentFile->next = file;
                currentFile = file;

            // Move past the separator or comma
            if (input[*index] == ',') 
                (*index)++;
        }

        // Move past the '@' separator
        (*index)++;

        // Recursively create next directories
        if (input[*index] == '\0') 
            directory->next = NULL;
        else 
            directory->next = loadTempDirectory(input, index);

        return directory;
    }

//
//  SAVING DATA TO DISK
//
////////////////////////////////////////////////