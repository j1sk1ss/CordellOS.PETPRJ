#include "../../include/file_system.h"


struct Directory* mainDirectory     = NULL;
struct Directory* currentDirectory  = NULL;

//
//  In sector(FILE_SYSTEM_SECTOR) placed LBA of sectors (13, 23, ..., n), that contains data about FS. 
//  
//


void init_directory() {
    char* loaded_data = ATA_read_sector(FILE_SYSTEM_SECTOR);

    if (loaded_data != NULL)
        if (ATA_is_current_sector_empty(FILE_SYSTEM_SECTOR) == false) {
            int index = 0;
            memset(index, 0, sizeof(index));
            set_main_directory(load_directory(loaded_data, index));
            return;
        }

    create_directory("root");

    mainDirectory = currentDirectory;
}

////////////////////////////////////////////////
//
//  CREATES TEMP DIRECTORY <NAME> IN CURRENT DIRECTORY
//

    void create_directory(char* name) {
        if (find_directory(name) != NULL) {
            printf("La directory esiste gia'.");
            return;
        }

        struct Directory* newDirectory = malloc(sizeof(struct Directory));
        memset(newDirectory, 0, sizeof(newDirectory));

        newDirectory->name = malloc(strlen(name));
        strcpy(newDirectory->name, name);
        
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
                struct Directory* current = currentDirectory->subDirectory;
                while (current->next != NULL) 
                    current = current->next;

                current->next = newDirectory;
            }
        }  

        save_file_system();
    }

//
//  CREATES TEMP DIRECTORY <NAME> IN CURRENT DIRECTORY
//
////////////////////////////////////////////////
//
//  CREATES TEMP FILE WITH NAME <NAME> IN CURRENT DIRECTORY
//

    void create_file(int type, char* name, uint8_t* head_sector) {
        if (find_file(name) != NULL) {
            printf("Il file esiste gia'.");
            return;
        }

        struct File* newFile = malloc(sizeof(struct File));
        memset(newFile, 0, sizeof(newFile));

        newFile->name = malloc(strlen(name));
        strcpy(newFile->name, name);

        newFile->fileType = type;

        newFile->sectors = (uint32_t*)malloc(sizeof(uint32_t));
        newFile->sector_count++;

        newFile->sectors[0] = head_sector;
        ATA_write_sector(head_sector, "\0");

        if (currentDirectory->files == NULL)
            currentDirectory->files = newFile;
        else {
            struct File* current = currentDirectory->files;
            while (current->next != NULL) 
                current = current->next;
            
            current->next = newFile;
        }

        save_file_system();
    }

    //  CREATE FILE
    ////////////
    //  WRITE TO FILE

        void write_file(struct File* file, char* data) {
            int data_len = strlen(data);
            
            // Iterate through the sectors of the file and write the data
            for (size_t i = 0; i < (int)(file->sector_count); i++) {
                int bytes_to_write = 512;
                if (data_len < bytes_to_write) 
                    bytes_to_write = data_len;

                ATA_clear_sector(file->sectors[i]);

                // Read the current sector's data into a buffer
                char* sector_data = (char*)malloc(bytes_to_write);
                memset(sector_data, 0, sizeof(sector_data));

                // Copy the data into the sector's data buffer
                strncpy(sector_data, data, bytes_to_write);

                // Write the modified sector data back to the sector
                ATA_write_sector(file->sectors[i], sector_data);

                data += bytes_to_write; // Move the data pointer to the next chunk
                data_len -= bytes_to_write;

                // If there's more data and we have exceeded the existing sectors, allocate a new sector
                if (data_len > 0 && i == (file->sector_count - 1)) {
                    int new_sector = ATA_find_empty_sector(0);
                    if (new_sector != -1) {
                        file->sectors = realloc(file->sectors, (file->sector_count + 1) * sizeof(uint32_t));
                        file->sectors[file->sector_count] = new_sector;
                        file->sector_count++;
                    }
                    else 
                        return;
                }
            }
        }

    //  WRITE TO FILE
    ////////////
    //  READ FROM FILE

        char* read_file(struct File* file) {
            char* data = (char*)malloc(512 * file->sector_count);

            for (size_t i = 0; i < file->sector_count; i++) 
                strcat(data, ATA_read_sector(file->sectors[i]));

            return data;
        }

    //  READ FROM FILE
    ////////////
    //  CLEAR FILE

        void clear_file(struct File* file) {
            char* empty_data[512];
            memset(empty_data, 0, sizeof(empty_data));

            for (size_t i = 0; i < file->sector_count; i++) 
                ATA_write_sector(file->sectors[i], empty_data);
        }

    //  CLEAR FILE
    ////////////
    //  CHECK FILE EXIST

        int file_exist(char* name) {
            if (find_file(name) == NULL)
                return -1;

            return 1;
        }

    //  CHECK FILE EXIST
    ////////////

//
//  CREATES TEMP FILE WITH NAME <NAME> IN CURRENT DIRECTORY
//
////////////////////////////////////////////////
//
//  DELETE TEMP DIRECTORY
//

    void delete_directory(char* name) {
        struct Directory* current   = currentDirectory->subDirectory;
        struct Directory* prev      = NULL;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {
                if (prev == NULL) currentDirectory->subDirectory = current->next;
                else prev->next = current->next;
                
                free(current->name);
                free(current);

                break;
            }

            prev    = current;
            current = current->next;
        }

        save_file_system();
    }

//
//  DELETE TEMP DIRECTORY
//
////////////////////////////////////////////////
//
//  DELETE TEMP FILE
//

    void delete_file(char* name) {
        struct File* current    = currentDirectory->files;
        struct File* prev       = NULL;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {
                clear_file(current);

                if (prev == NULL) currentDirectory->files = current->next;
                else prev->next = current->next;
                
                free(current);
                
                break;
            }

            prev    = current;
            current = current->next;
        }

        save_file_system();
    }

//
//  DELETE TEMP FILE
//
////////////////////////////////////////////////
//
//  OTHER FUNCTIONS
//

    struct File* find_file(char* name) {
        struct File* current = currentDirectory->files;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) 
                return current;
            
            current = current->next;
        }

        return NULL;
    }

    struct Directory* find_directory(char* name) {
        struct Directory* current = currentDirectory->subDirectory;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) 
                return current;

            current = current->next;
        }

        return NULL;
    }

    void move_to_directory(char* name) {
        struct Directory* neededDirectory = find_directory(name);
        if (neededDirectory != NULL) 
            currentDirectory = neededDirectory;
    }

    void up_from_directory() {
        if (currentDirectory->upDirectory != NULL)
            currentDirectory = currentDirectory->upDirectory;
    }

    struct Directory* get_current_directory() {
        return currentDirectory;
    }

    char* get_full_temp_name() {
        char* name = malloc(0);
        memset(name, 0, sizeof(name));

        struct Directory* current = currentDirectory;

        while (current != NULL) {
            char* temp_name = malloc(sizeof(name) + sizeof(current->name) + 1);
            memset(temp_name, 0, sizeof(temp_name));
            if (temp_name == NULL) {
                printf("[TFS 304] Alloc error!");
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

    void set_main_directory(struct Directory* directory) {
        mainDirectory    = directory;
        currentDirectory = directory;
    }

    struct Directory* get_main_directory() {
        return mainDirectory;
    }

//
//  OTHER FUNCTIONS
//
////////////////////////////////////////////////
//
//  SAVING DATA TO DISK
//

    void save_file_system() {
        ATA_clear_sector(FILE_SYSTEM_SECTOR);

        char result[512];
        memset(result, 0, sizeof(result));

        save_directory(get_main_directory(), result);

        if (ATA_write_sector(FILE_SYSTEM_SECTOR, result) == -1)
            printf("\n\rError while writing to disk. Please try again");
    }

    void save_directory(struct Directory* directory, char* result) {
        if (directory == NULL) 
            return;

        strcat(result, "D");
        strcat(result, directory->name);

        if (directory->subDirectory != NULL) {
            strcat(result, "N");
            save_directory(directory->subDirectory, result);
        }

        struct File* file = directory->files;
        if (file != NULL)
            strcat(result, "N");

        while (file != NULL) {
            strcat(result, "F");
            strcat(result, file->name);
            strcat(result, "T");
            strcat(result, fprintf_unsigned(-1, file->fileType, 10, 0));
            strcat(result, "S");

            int sector_count = file->sector_count;
            for (size_t i = 0; i < sector_count; i++) {
                strcat(result, fprintf_unsigned(-1, file->sectors[i], 10, 0));

                if (i < sector_count - 1 && file->sectors[i + 1] != -1) 
                    strcat(result, "S");
            }

            file = file->next;
            if (file != NULL) 
                strcat(result, "#");
        }

        strcat(result, "@");

        if (directory->next != NULL) {
            strcat(result, "#");
            save_directory(directory->next, result);
        }
    }

    struct File* load_temp_file(const char* input, int* index) {
        struct File* file = (struct File*)malloc(sizeof(struct File));

        file->fileType      = NULL;
        file->name          = NULL;
        file->sector_count  = 0;
        
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

                file->fileType = input[(*index)++] - '0';
                
                while (input[*index] == 'S') {
                    (*index)++; // Move past 'S'
                    
                    start = *index;
                    while (input[*index] != '\0' && (input[*index] >= '0' && input[*index] <= '9')) 
                        (*index)++;

                    length = *index - start;
                    if (length > 0) {
                        char sectorStr[10];
                        strncpy(sectorStr, input + start, length);
                        sectorStr[length] = '\0';

                        reverse(sectorStr, strlen(sectorStr));
                        int sector = atoi(sectorStr);

                        file->sector_count++;
                        file->sectors = realloc(file->sectors, file->sector_count * sizeof(uint32_t));
                        file->sectors[file->sector_count - 1] = (uint32_t)sector;
                    }
                }
            }
        }

        return file;
    }

    struct Directory* load_directory(const char* input, int* index) {
        struct Directory* directory = (struct Directory*)malloc(sizeof(struct Directory));

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
            memset(dirName, 0, sizeof(dirName));

            strncpy(dirName, input + start, *index - start);
            dirName[*index - start] = '\0';
            directory->name = strdup(dirName);
        }

        while (input[*index] == 'N') {
            (*index)++;

            if (input[*index] == 'D') {
                directory->subDirectory = load_directory(input, index);
                directory->subDirectory->upDirectory = directory;
            }

            if (input[*index] == 'F') 
                directory->files = load_temp_file(input, index);
        }

        if (input[*index] == '@') 
            (*index)++;

        while (input[*index] == '#') {
            (*index)++;
            if (input[*index] == 'D') {
                directory->next = load_directory(input, index);
                directory->next->upDirectory = directory;
            }

            if (input[*index] == 'F') {
                struct File* end_file = directory->files;
                while (end_file->next != NULL)
                    end_file = end_file->next;

                end_file->next = load_temp_file(input, index);
            }
        }

        return directory;
    }

//
//  SAVING DATA TO DISK
//
////////////////////////////////////////////////