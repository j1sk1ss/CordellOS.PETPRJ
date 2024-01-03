#include "../../include/file_system.h"


struct Directory* mainDirectory     = NULL;
struct Directory* currentDirectory  = NULL;

//  _____ ___ _     _____    ______   ______ _____ _____ __  __ 
// |  ___|_ _| |   | ____|  / ___\ \ / / ___|_   _| ____|  \/  |
// | |_   | || |   |  _|    \___ \\ V /\___ \ | | |  _| | |\/| |
// |  _|  | || |___| |___    ___) || |  ___) || | | |___| |  | |
// |_|   |___|_____|_____|  |____/ |_| |____/ |_| |_____|_|  |_|
                                                              
//
//  In sector(FILE_SYSTEM_SECTOR) placed LBA of sectors (13 23 ... n), that contains data about FS. 
//  Every file contains list of data of sectors, that contains file data
//


void FS_init() {

    if (ATA_is_current_sector_empty(FILE_SYSTEM_SECTOR) == false) {
        FS_load_file_system();
        return;
    }

        ////////////////////////////
        // STATUS CHECKING
        // - FILE_SYSTEM_SECTOR
        // - USERS_SECTOR
        // - GROUPS_SECTOR
        // - SHELL_SECTOR

            if (ATA_is_current_sector_empty(FILE_SYSTEM_SECTOR) == true)
                cprintf(FOREGROUND_GREEN, "\r\nSETTORE FILE SYSTEM [%i] E` CHIARO", FILE_SYSTEM_SECTOR);
            else {
                cprintf(FOREGROUND_RED, "\r\nSETTORE FILE SYSTEM [%i] NON CHIARO \n TENTATIVO DI PULIRE...", FILE_SYSTEM_SECTOR);
                if (ATA_clear_sector(FILE_SYSTEM_SECTOR) == 1) 
                    cprintf(FOREGROUND_GREEN, "\r\nSETTORE FILE SYSTEM [%i] E` CANCELLATO", FILE_SYSTEM_SECTOR);
                else cprintf(FOREGROUND_RED, "\r\nSETTORE FILE SYSTEM [%i] NON DISPONIBILE PER LA PULIZIA", FILE_SYSTEM_SECTOR);
            }


            if (ATA_is_current_sector_empty(USERS_SECTOR) == true)
                cprintf(FOREGROUND_GREEN, "\r\nSETTORE UTENTI [%i] E` CHIARO", USERS_SECTOR);
            else {
                cprintf(FOREGROUND_RED, "\r\nSETTORE UTENTI [%i] NON CHIARO \n TENTATIVO DI PULIRE...", USERS_SECTOR);
                if (ATA_clear_sector(USERS_SECTOR) == 1)
                    cprintf(FOREGROUND_GREEN, "\r\nSETTORE UTENTI [%i] E` CANCELLATO", USERS_SECTOR);
                else cprintf(FOREGROUND_RED, "\r\nnSETTORE UTENTI [%i] NON DISPONIBILE PER LA PULIZIA", USERS_SECTOR);
            }


            if (ATA_is_current_sector_empty(GROUPS_SECTOR) == true)
                cprintf(FOREGROUND_GREEN, "\r\nSETTORE GRUPPI [%i] E` CHIARO", GROUPS_SECTOR);
            else {
                cprintf(FOREGROUND_RED, "\r\nSETTORE GRUPPI [%i] NON CHIARO \n TENTATIVO DI PULIRE...", GROUPS_SECTOR);
                if (ATA_clear_sector(GROUPS_SECTOR) == 1)
                    cprintf(FOREGROUND_GREEN, "\r\nSETTORE GRUPPI [%i] E` CANCELLATO", GROUPS_SECTOR);
                else cprintf(FOREGROUND_RED, "\r\nnSETTORE GRUPPI [%i] NON DISPONIBILE PER LA PULIZIA", GROUPS_SECTOR);
            }


            if (ATA_is_current_sector_empty(SHELL_SECTOR) == true)
                cprintf(FOREGROUND_GREEN, "\r\nSETTORE GUSCIO [%i] E` CHIARO", SHELL_SECTOR);
            else {
                cprintf(FOREGROUND_RED, "\r\nSETTORE GUSCIO [%i] NON CHIARO \n TENTATIVO DI PULIRE...", SHELL_SECTOR);
                if (ATA_clear_sector(SHELL_SECTOR) == 1)
                    cprintf(FOREGROUND_GREEN, "\r\nSETTORE GUSCIO [%i] E` CANCELLATO", SHELL_SECTOR);
                else cprintf(FOREGROUND_RED, "\r\nnSETTORE GUSCIO [%i] NON DISPONIBILE PER LA PULIZIA", SHELL_SECTOR);
            }


            printf("\n\nFAL FS Initialized\n\n");

        ////////////////////////////

    FS_create_directory("~");
    mainDirectory = currentDirectory;
}

void FS_load_file_system() {
    char* loaded_data = ATA_read_sector(FILE_SYSTEM_SECTOR, 1);
    if (loaded_data != NULL) {
        char* token = strtok(loaded_data, " ");
        char* file_system_data = NULL;
        while(token != NULL) {
            char* sector_data = ATA_read_sector(atoi(token), 1);
            file_system_data = (char*)realloc(file_system_data, strlen(sector_data) * sizeof(char));
            strcat(file_system_data, sector_data);

            token = strtok(NULL, " ");
            free(sector_data);
        }

        int index = 0;
        memset(index, 0, sizeof(index));
        FS_set_main_directory(FS_load_directory(file_system_data, index));

        free(loaded_data);
    }
}

void FS_unload_file_system(struct Directory* directory) {
    struct File* current_file = directory->files;
    struct File* next_file    = NULL;
    while (current_file != NULL) {
        free(current_file->sectors);

        next_file = current_file->next;
        free(current_file);
        current_file = next_file;
    }

    if (directory->subDirectory != NULL)
        FS_unload_file_system(directory->subDirectory);

    if (directory->next != NULL) 
        FS_unload_file_system(directory->next);

    free(directory);
    FS_set_main_directory(NULL);
}

////////////////////////////
// FIND FILES AND DIRECTORIES

    // Find file in directory
    struct File* FS_find_file(char* name, struct Directory* directory) {
        if (directory == NULL) return NULL;

        struct File* current = directory->files;
        while (current != NULL) {
            if (strstr(current->name, name) == 0) 
                return current;
            
            current = current->next;
        }

        return current;
    }

    // Find directory in direcotry
    struct Directory* FS_find_directory(char* name, struct Directory* directory) {
        if (directory == NULL) return NULL;

        struct Directory* current = directory->subDirectory;
        while (current != NULL) {
            if (strstr(current->name, name) == 0) 
                return current;

            current = current->next;
        }

        return current;
    }

    // Find directory in all FS by path
    struct Directory* FS_global_find_directory(char* path) {
        char* token = strtok(path, "/");
        struct Directory* current = mainDirectory;
        if (current != NULL)
            while(token != NULL) {
                struct Directory* sub_directory = FS_find_directory(token, current);
                if (sub_directory != NULL) 
                    current = sub_directory;
                else return NULL;

                token = strtok(NULL, "/");
            }
        else return NULL;

        return current;
    }

    // Find file in all FS by path
    struct File* FS_global_find_file(char* path) {
        char* dir_path = (char*)malloc(strlen(path));
        strcpy(dir_path, path);

        char* file_name = strrchr(path, '/');
        file_name++;

        char* token = strtok(dir_path, "/");
        struct File* file = NULL;
        struct Directory* current = mainDirectory;

        while (token != NULL) {
            if (current != NULL) 
                if (strstr(token, file_name) != 0)
                    current = FS_find_directory(token, current);
                else break;
            else break;

            token = strtok(NULL, "/");
        }

        return FS_find_file(file_name, current);
    }

// FIND FILES AND DIRECTORIES
////////////////////////////

////////////////////////////////////////////////
//
//  CREATES TEMP DIRECTORY <NAME> IN CURRENT DIRECTORY
//

    void FS_create_directory(char* name) {
        struct Directory** directory = &currentDirectory;
        if (name[0] == '/') {
            char* dir_path = (char*)malloc(strlen(name));
            strcpy(dir_path, name);

            char* dir_name = strrchr(name, '/');
            dir_name++;

            char* token = strtok(dir_path, "/");
            struct Directory* current = mainDirectory;

            while (token != NULL) {
                if (current != NULL) 
                    if (strstr(token, dir_name) != 0)
                        current = FS_find_directory(token, current);
                    else break;
                else break;

                token = strtok(NULL, "/");
            }

            directory = &current;
            name = dir_name;
            free(dir_path);
        }
        
        if (FS_find_directory(name, *directory) != NULL) {
            printf("La directory esiste gia'");
            return;
        }

        struct Directory* newDirectory = malloc(sizeof(struct Directory));
        memset(newDirectory, 0, sizeof(newDirectory));

        strncpy(newDirectory->name, name, 11);
        newDirectory->files         = NULL;
        newDirectory->next          = NULL;
        newDirectory->subDirectory  = NULL;

        if (*directory == NULL) 
            *directory = newDirectory;
        else {
            newDirectory->upDirectory = *directory;
            if ((*directory)->subDirectory == NULL) 
                (*directory)->subDirectory = newDirectory;
            else {
                struct Directory* current = (*directory)->subDirectory;
                while (current->next != NULL) 
                    current = current->next;

                current->next = newDirectory;
            }
        }  

        FS_save_file_system();
    }

//
//  CREATES TEMP DIRECTORY <NAME> IN CURRENT DIRECTORY
//
////////////////////////////////////////////////
//
//  CREATES TEMP FILE WITH NAME <NAME> IN CURRENT DIRECTORY
//

    void FS_create_file(int read, int write, int edit, char* name, char* extension, uint8_t* head_sector) {
        struct Directory** directory = &currentDirectory;
        if (name[0] == '/') {
            char* dir_path = (char*)malloc(strlen(name));
            strcpy(dir_path, name);

            char* file_name = strrchr(name, '/');
            file_name++;

            char* token = strtok(dir_path, "/");
            struct Directory* current = mainDirectory;

            while (token != NULL) {
                if (current != NULL) 
                    if (strstr(token, file_name) != 0)
                        current = FS_find_directory(token, current);
                    else break;
                else break;

                token = strtok(NULL, "/");
            }

            directory = &current;
            name = file_name;
            free(dir_path);
        }
        
        if (FS_find_file(name, *directory) != NULL) {
            printf("Il file esiste gia'.");
            return;
        }

        struct File* newFile = malloc(sizeof(struct File));
        memset(newFile, 0, sizeof(newFile));

        strncpy(newFile->name, name, 11);
        strncpy(newFile->extension, extension, 3);

        newFile->read_level     = max(min(9, read), 0);
        newFile->write_level    = max(min(9, write), 0);
        newFile->edit_level     = max(min(9, edit), 0);

        newFile->sectors = (uint32_t*)malloc(sizeof(uint32_t));
        newFile->sector_count++;

        newFile->sectors[0] = head_sector;
        ATA_write_sector(head_sector, 1, "\0");

        if ((*directory)->files == NULL)
            (*directory)->files = newFile;
        else {
            struct File* current = (*directory)->files;
            while (current->next != NULL) 
                current = current->next;
            
            current->next = newFile;
        }

        FS_save_file_system();
    }

    //  CREATE FILE
    ////////////
    //  WRITE TO FILE

        void FS_write_file(struct File* file, char* data) {
            int data_len = strlen(data);

            // Iterate through the sectors of the file and write the data
            size_t sectors_written = 0;
            for (size_t i = 0; i < file->sector_count; i++) {
                int bytes_to_write = SECTOR_SIZE;
                if (data_len < bytes_to_write) 
                    bytes_to_write = data_len;

                ATA_clear_sector(file->sectors[i]);

                // Read the current sector's data into a buffer
                char* sector_data = (char*)malloc(bytes_to_write + 1);
                memset(sector_data, 0, bytes_to_write + 1);

                // Copy the data into the sector's data buffer
                strncpy(sector_data, data, bytes_to_write);
                ATA_write_sector(file->sectors[i], 1, sector_data);

                data += bytes_to_write; // Move the data pointer to the next chunk
                data_len -= bytes_to_write;

                sectors_written++;
                // If there's more data and we have exceeded the existing sectors, allocate a new sector
                if (data_len <= 0) break;
                
                if (data_len > 0 && i == (file->sector_count - 1)) {
                    int new_sector = ATA_find_empty_sector(FILES_SECTOR_OFFSET);
                    if (new_sector != -1) {
                        file->sectors = realloc(file->sectors, (file->sector_count + 1) * sizeof(uint32_t));
                        file->sectors[file->sector_count] = new_sector;
                        file->sector_count++;
                    } else {
                        // Handle the case where you couldn't find an empty sector
                        free(sector_data); // Free the allocated memory
                        return;
                    }
                }

                free(sector_data); // Free the allocated memory for sector_data
            }

            // Update the file's sector count and memory allocation
            file->sector_count = sectors_written;
            file->sectors = realloc(file->sectors, file->sector_count * sizeof(uint32_t));
            
            // Save the file system
            FS_save_file_system();
        }

    //  WRITE TO FILE
    ////////////
    //  READ FROM FILE

        char* FS_read_file(struct File* file) {
            char* data = (char*)malloc(512 * file->sector_count);

            for (size_t i = 0; i < file->sector_count; i++) {
                char* sector_data = ATA_read_sector(file->sectors[i], 1);
                strcat(data, sector_data);
                free(sector_data);
            }

            return data;
        }

    //  READ FROM FILE
    ////////////
    //  CLEAR FILE

        void FS_clear_file(struct File* file) {
            char* empty_data[512];
            memset(empty_data, 0, sizeof(empty_data));

            for (size_t i = 0; i < file->sector_count; i++) 
                ATA_write_sector(file->sectors[i], 1, empty_data);
        }

    //  CLEAR FILE
    ////////////
    //  CHECK FILE EXIST

        int FS_file_exist(char* name) {
            if (name[0] == '/') 
                if (FS_global_find_file(name) == NULL)
                    return -1;

            if (FS_find_file(name, FS_get_current_directory()) == NULL)
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

    void FS_delete_directory(char* name, struct Directory* directory) {
        struct Directory* current   = directory->subDirectory;
        struct Directory* prev      = NULL;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {
                if (prev == NULL) directory->subDirectory = current->next;
                else prev->next = current->next;
                
                free(current);
                break;
            }

            prev    = current;
            current = current->next;
        }

        FS_save_file_system();
    }

//
//  DELETE TEMP DIRECTORY
//
////////////////////////////////////////////////
//
//  DELETE TEMP FILE
//

    void FS_delete_file(char* name, struct Directory* directory) {
        struct File* current    = directory->files;
        struct File* prev       = NULL;

        while (current != NULL) {
            if (strcmp(current->name, name) == 0) {
                FS_clear_file(current);

                if (prev == NULL) directory->files = current->next;
                else prev->next = current->next;
                
                free(current);
                break;
            }

            prev    = current;
            current = current->next;
        }

        FS_save_file_system();
    }

//
//  DELETE TEMP FILE
//
////////////////////////////////////////////////
//
//  OTHER FUNCTIONS
//

    // Move to sub directory of current directory
    int FS_move_to_directory(char* name, struct Directory* directory) {
        struct Directory* neededDirectory;
        if (name[0] == '/') neededDirectory = FS_global_find_directory(name);
        else neededDirectory = FS_find_directory(name, directory);
        
        if (neededDirectory != NULL) 
            currentDirectory = neededDirectory;
        else return -1;
    }

    void FS_up_from_directory() {
        if (currentDirectory->upDirectory != NULL) 
            currentDirectory = currentDirectory->upDirectory;
    }

    struct Directory* FS_get_current_directory() {
        return currentDirectory;
    }

    char* FS_get_full_temp_name() {
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

            strcpy(temp_name, current->name);
            temp_name = strcat(temp_name, "/");
            temp_name = strcat(temp_name, name);
        
            free(name);
            name = temp_name;
            current = current->upDirectory;
        }

        return name;
    }

    void FS_set_main_directory(struct Directory* directory) {
        mainDirectory    = directory;
        currentDirectory = directory;
    }

    struct Directory* FS_get_main_directory() {
        return mainDirectory;
    }

//
//  OTHER FUNCTIONS
//
////////////////////////////////////////////////
//
//  SAVING DATA TO DISK
//

    void FS_save_file_system() {
        if (ATA_is_current_sector_empty(FILE_SYSTEM_SECTOR) == false) {
            char* sector_data = ATA_read_sector(FILE_SYSTEM_SECTOR, 1);
            char* token = strtok(sector_data, " ");
            while(token != NULL) {
                ATA_clear_sector(atoi(token));
                token = strtok(NULL, " ");
            }

            free(sector_data);
        }

        ATA_clear_sector(FILE_SYSTEM_SECTOR);

        char* result = FS_save_directory(FS_get_main_directory());
        int data_len = strlen(result);
        while (data_len > 0) {
            int bytes_to_write = SECTOR_SIZE;
            if (data_len < bytes_to_write) 
                bytes_to_write = data_len;

            // Write the modified sector data back to the sector
            int sector = ATA_find_empty_sector(SYS_FILES_SECTOR_OFFSET);
            ATA_write_sector(sector, 1, result);
            ATA_append_sector(FILE_SYSTEM_SECTOR, itoa(sector));
            ATA_append_sector(FILE_SYSTEM_SECTOR, " ");

            result += bytes_to_write; // Move the data pointer to the next chunk
            data_len -= bytes_to_write;
        }

        free(result);
    }

    char* FS_save_directory(struct Directory* directory) {
        char* result = (char*)malloc(sizeof(char));
        if (directory == NULL) 
            return;

        result = realloc(result, strlen(result) * sizeof(char) + sizeof(char));
        strcat(result, "D");

        result = realloc(result, strlen(result) * sizeof(char) + strlen(directory->name) * sizeof(char));
        strcat(result, directory->name);

        if (directory->subDirectory != NULL) {
            result = realloc(result, strlen(result) * sizeof(char) + sizeof(char));
            strcat(result, "N");

            char* sub_result = FS_save_directory(directory->subDirectory);
            result = realloc(result, strlen(result) * sizeof(char) + strlen(sub_result) * sizeof(char));
            strcat(result, sub_result);
        }

        struct File* file = directory->files;
        if (file != NULL && directory->subDirectory == NULL) {
            result = realloc(result, strlen(result) * sizeof(char) + sizeof(char));
            strcat(result, "N");
        } else if (file != NULL && directory->subDirectory != NULL) {
            result = realloc(result, strlen(result) * sizeof(char) + sizeof(char));
            strcat(result, "#");
        }

        while (file != NULL) {
            result = realloc(result, strlen(result) * sizeof(char) + sizeof(char));
            strcat(result, "F");

            result = realloc(result, strlen(result) * sizeof(char) + strlen(file->name) * sizeof(char));
            strcat(result, file->name);

            result = realloc(result, strlen(result) * sizeof(char) + 5 * sizeof(char));
            strcat(result, "T");

            strcat(result, itoa(file->read_level));
            strcat(result, itoa(file->write_level));
            strcat(result, itoa(file->edit_level));

            strcat(result, "S");
            size_t sector_count = file->sector_count;
            for (size_t i = 0; i < sector_count; i++) {
                char* sector_address = itoa(file->sectors[i]);
                result = realloc(result, strlen(result) * sizeof(char) + strlen(sector_address) * sizeof(char));
                strcat(result, itoa(file->sectors[i]));

                if (i < sector_count - 1) {
                    result = realloc(result, strlen(result) * sizeof(char) + sizeof(char));
                    strcat(result, "S");
                }

                free(sector_address);
            }
            
            strcat(result, "E");
            strcat(result, file->extension);

            file = file->next;
            if (file != NULL) {
                result = realloc(result, strlen(result) * sizeof(char) + sizeof(char));
                strcat(result, "#");
            }
        }

        result = realloc(result, strlen(result) * sizeof(char) + sizeof(char));
        strcat(result, "@");

        if (directory->next != NULL) {
            result = realloc(result, strlen(result) * sizeof(char) + sizeof(char));
            strcat(result, "#");

            char* next_result = FS_save_directory(directory->next);
            result = realloc(result, strlen(result) * sizeof(char) + strlen(next_result) * sizeof(char));
            strcat(result, next_result);
        }

        return result;
    }

    struct File* FS_load_temp_file(const char* input, int* index) {
        struct File* file = (struct File*)malloc(sizeof(struct File));
        file->sector_count  = 0;
        
        if (input[*index] == 'F') {
            (*index)++; // Move past 'F'
            
            int start = *index;
            while (input[*index] != '\0' && input[*index] != 'T') 
                (*index)++;

            int length = *index - start;
            if (length > 0) {
                strncpy(file->name, input + start, length);
                file->name[length] = '\0';
            }
            
            if (input[*index] == 'T') {
                (*index)++; // Move past 'T'

                file->read_level    = input[(*index)++] - '0';
                file->write_level   = input[(*index)++] - '0';
                file->edit_level    = input[(*index)++] - '0';
                
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

                        int sector = atoi(sectorStr);
                        file->sector_count++;
                        file->sectors = realloc(file->sectors, file->sector_count * sizeof(uint32_t));
                        file->sectors[file->sector_count - 1] = (uint32_t)sector;
                    }
                }
            }

            if (input[*index] == 'E') {
                (*index)++;

                start = *index;
                while (input[*index] != '\0' && input[*index] != '@' && input[*index] != '#') 
                    (*index)++;

                length = *index - start;
                if (length > 0) 
                    strncpy(file->extension, input + start, min(length, 3));
            }
        }

        return file;
    }

    struct Directory* FS_load_directory(const char* input, int* index) {
        struct Directory* directory = (struct Directory*)malloc(sizeof(struct Directory));

        directory->files        = NULL;
        directory->subDirectory = NULL;
        directory->upDirectory  = NULL;
        directory->next         = NULL;

        ////////////////////
        //  DIR NAME
        //

            if (input[*index] == 'D') {
                (*index)++;
                int start = *index;
                while (input[*index] != '\0' && input[*index] != '@' && input[*index] != '#' && input[*index] != 'N') 
                    (*index)++;

                char dirName[12];
                memset(dirName, 0, sizeof(dirName));

                strncpy(dirName, input + start, *index - start);
                dirName[*index - start] = '\0';
                strncpy(directory->name, dirName, 11);
            }

        //
        //  DIR NAME
        ////////////////////

        if (input[*index] == '@') {
            (*index)++;
            return directory;
        }

        ////////////////////
        //  SUB FILES \ DIRS
        //

            while (input[*index] == 'N') {
                (*index)++;

                if (input[*index] == 'D') {
                    directory->subDirectory = FS_load_directory(input, index);
                    directory->subDirectory->upDirectory = directory;
                }

                if (input[*index] == 'F') 
                    directory->files = FS_load_temp_file(input, index);
            }

        //
        //  SUB FILES \ DIRS
        ////////////////////

        if (input[*index] == '@') {
            (*index)++;
            return directory;
        }

        ////////////////////
        //  ADDITIONAL FILES \ DIRS
        //

            while (input[*index] == '#') {
                (*index)++;

                if (input[*index] == 'D') {
                    struct Directory* end_dir = directory->subDirectory;
                    while (end_dir->next != NULL)
                        end_dir = end_dir->next;

                    end_dir->next = FS_load_directory(input, index);
                    end_dir->next->upDirectory = directory;
                }

                if (input[*index] == 'F') {
                    struct File* end_file = directory->files;
                    if (end_file == NULL) {
                        directory->files = FS_load_temp_file(input, index);
                        continue;
                    }
                    else
                        while (end_file->next != NULL)
                            end_file = end_file->next;

                    end_file->next = FS_load_temp_file(input, index);
                }
            }

        //
        //  ADDITIONAL FILES \ DIRS
        ////////////////////

        if (input[*index] == '@') 
            (*index)++;

        return directory;
    }

//
//  SAVING DATA TO DISK
//
////////////////////////////////////////////////