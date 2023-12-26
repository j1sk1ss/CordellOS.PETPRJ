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

void init_directory() {
    char* loaded_data = ATA_read_sector(FILE_SYSTEM_SECTOR);
    
    if (loaded_data != NULL)
        if (ATA_is_current_sector_empty(FILE_SYSTEM_SECTOR) == false) {
            char* token = strtok(loaded_data, " ");
            char* file_system_data = NULL;

            while(token != NULL) {
                char* sector_data = ATA_read_sector(atoi(token));
                file_system_data = (char*)realloc(file_system_data, strlen(sector_data) * sizeof(char));
                strcat(file_system_data, sector_data);

                token = strtok(NULL, " ");
                free(sector_data);
            }

            int index = 0;
            memset(index, 0, sizeof(index));
            set_main_directory(load_directory(file_system_data, index));

            free(loaded_data);
            return;
        }

    create_directory("home");
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

        strncpy(newDirectory->name, name, 11);
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

    void create_file(int read, int write, int edit, char* name, char* extension, uint8_t* head_sector) {
        if (find_file(name) != NULL) {
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
                ATA_write_sector(file->sectors[i], sector_data);

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
            save_file_system();
        }

    //  WRITE TO FILE
    ////////////
    //  READ FROM FILE

        char* read_file(struct File* file) {
            char* data = (char*)malloc(512 * file->sector_count);

            for (size_t i = 0; i < file->sector_count; i++) {
                char* sector_data = ATA_read_sector(file->sectors[i]);
                strcat(data, sector_data);
                free(sector_data);
            }

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
            if (strstr(current->name, name) == 0) 
                return current;
            
            current = current->next;
        }

        return NULL;
    }

    struct Directory* find_directory(char* name) {
        struct Directory* current = currentDirectory->subDirectory;

        while (current != NULL) {
            if (strstr(current->name, name) == 0) 
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

            strcpy(temp_name, current->name);
            temp_name = strcat(temp_name, "/");
            temp_name = strcat(temp_name, name);
        
            free(name);
            name = temp_name;
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
        if (ATA_is_current_sector_empty(FILE_SYSTEM_SECTOR) == false) {
            char* sector_data = ATA_read_sector(FILE_SYSTEM_SECTOR);
            char* token = strtok(sector_data, " ");
            while(token != NULL) {
                ATA_clear_sector(atoi(token));
                token = strtok(NULL, " ");
            }

            free(sector_data);
        }

        ATA_clear_sector(FILE_SYSTEM_SECTOR);

        char* result = save_directory(get_main_directory());
        int data_len = strlen(result);
        while (data_len > 0) {
            int bytes_to_write = SECTOR_SIZE;
            if (data_len < bytes_to_write) 
                bytes_to_write = data_len;

            // Write the modified sector data back to the sector
            int sector = ATA_find_empty_sector(SYS_FILES_SECTOR_OFFSET);
            ATA_write_sector(sector, result);
            ATA_append_sector(FILE_SYSTEM_SECTOR, itoa(sector));
            ATA_append_sector(FILE_SYSTEM_SECTOR, " ");

            result += bytes_to_write; // Move the data pointer to the next chunk
            data_len -= bytes_to_write;
        }

        free(result);
    }

    char* save_directory(struct Directory* directory) {
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

            char* sub_result = save_directory(directory->subDirectory);
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

            char* next_result = save_directory(directory->next);
            result = realloc(result, strlen(result) * sizeof(char) + strlen(next_result) * sizeof(char));
            strcat(result, next_result);
        }

        return result;
    }

    struct File* load_temp_file(const char* input, int* index) {
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

    struct Directory* load_directory(const char* input, int* index) {
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
                    directory->subDirectory = load_directory(input, index);
                    directory->subDirectory->upDirectory = directory;
                }

                if (input[*index] == 'F') 
                    directory->files = load_temp_file(input, index);
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

                    end_dir->next = load_directory(input, index);
                    end_dir->next->upDirectory = directory;
                }

                if (input[*index] == 'F') {
                    struct File* end_file = directory->files;
                    if (end_file == NULL) {
                        directory->files = load_temp_file(input, index);
                        continue;
                    }
                    else
                        while (end_file->next != NULL)
                            end_file = end_file->next;

                    end_file->next = load_temp_file(input, index);
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