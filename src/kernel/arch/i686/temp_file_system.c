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

    void serializeTempFile(struct TempFile* file, char* buffer) {
        char sector_data[512];
        readSector(file->sector, sector_data);

        // Serialize fileType, name, and content
        memcpy(buffer, file->fileType, sizeof(file->fileType));
        memcpy(buffer + sizeof(file->fileType), file->name, sizeof(file->name));
        memcpy(buffer + sizeof(file->fileType) + sizeof(file->name), sector_data, sizeof(sector_data));
    }

    void deserializeTempFile(char* buffer, struct TempFile* file) {
        // Deserialize fileType, name, and content
        memcpy(file->fileType, buffer, sizeof(file->fileType));
        memcpy(file->name, buffer + sizeof(file->fileType), sizeof(file->name));
        memcpy(file->sector, buffer + sizeof(file->fileType) + sizeof(file->name), sizeof(file->sector));
    }

    // Function to serialize a TempDirectory to a buffer
    void serializeTempDirectory(struct TempDirectory* directory, char* buffer) {
        // Serialize name
        memcpy(buffer, directory->name, sizeof(directory->name));
        
        // Serialize and save the next TempFile (if available)
        if (directory->files) {
            serializeTempFile(directory->files, buffer + sizeof(directory->name));
        } else {
            memset(buffer + sizeof(directory->name), 0, sizeof(struct TempFile));
        }
        
        // Serialize and save the next TempDirectory (if available)
        if (directory->next) {
            serializeTempDirectory(directory->next, buffer + sizeof(directory->name) + sizeof(struct TempFile));
        } else {
            memset(buffer + sizeof(directory->name) + sizeof(struct TempFile), 0, sizeof(struct TempDirectory));
        }
        
        // Serialize and save the subdirectory (if available)
        if (directory->subDirectory) {
            serializeTempDirectory(directory->subDirectory, buffer + sizeof(directory->name) + sizeof(struct TempFile) + sizeof(struct TempDirectory));
        } else {
            memset(buffer + sizeof(directory->name) + sizeof(struct TempFile) + sizeof(struct TempDirectory), 0, sizeof(struct TempDirectory));
        }
    }

    // Function to deserialize a buffer into a TempDirectory
    void deserializeTempDirectory(char* buffer, struct TempDirectory* directory) {
        // Deserialize name
        memcpy(directory->name, buffer, sizeof(directory->name));
        
        // Deserialize the next TempFile
        deserializeTempFile(buffer + sizeof(directory->name), directory->files);
        
        // Deserialize the next TempDirectory
        deserializeTempDirectory(buffer + sizeof(directory->name) + sizeof(struct TempFile), directory->next);
        
        // Deserialize the subdirectory
        deserializeTempDirectory(buffer + sizeof(directory->name) + sizeof(struct TempFile) + sizeof(struct TempDirectory), directory->subDirectory);
    }

    // Function to save a TempDirectory to the disk
    void saveTempDirectory(uint32_t LBA, struct TempDirectory* directory) {
        char buffer[512];  // Assuming 512-byte sectors
        serializeTempDirectory(directory, buffer);
        writeSector(LBA, buffer);
    }

    // Function to load a TempDirectory from the disk
    void loadTempDirectory(uint32_t LBA, struct TempDirectory* directory) {
        char buffer[512];  // Assuming 512-byte sectors
        readSector(LBA, buffer);
        deserializeTempDirectory(buffer, directory);
    }

//
//  SAVING DATA TO DISK
//
////////////////////////////////////////////////