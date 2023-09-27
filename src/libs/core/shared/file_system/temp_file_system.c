#include "../include/string.h"


struct File {
    char* fileType;

    char* name;
    char* content;

    struct File* next;
};

struct Directory  {
    char* name;

    struct File* files;

    struct Directory* next;

    struct Directory* subDirectory;
    struct Directory* upDirectory;
};

struct Directory* currentDirectory;

char* get_full_temp_name() {
    char* name = malloc(0);
    memset(name, 0, sizeof(name));

    struct Directory* current = currentDirectory;

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

void init_directory() {
    currentDirectory = NULL;
    create_temp_directory("root");
}

////////////////////////////////////////////////
//
//  CREATES TEMP DIRECTORY <NAME> IN CURRENT DIRECTORY
//

    void create_temp_directory(char* name) {
        struct Directory* newDirectory = malloc(sizeof(struct Directory));
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
                struct Directory* current = currentDirectory->subDirectory;
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

    void create_temp_file(char* type, char* name, char** content) {
        struct File* newFile = malloc(sizeof(struct File));
        memset(newFile, 0, sizeof(newFile));

        newFile->name = malloc(strlen(name));;
        memcpy(newFile->name, name, strlen(name));

        newFile->fileType   = type;
        newFile->content    = content;

        if (currentDirectory->files == NULL)
            currentDirectory->files = newFile;
        else {
            struct File* current = currentDirectory->files;
            while (current->next != NULL) 
                current = current->next;
            
            current->next = newFile;
        }
    }

//
//  CREATES TEMP FILE WITH NAME <NAME> IN CURRENT DIRECTORY
//
////////////////////////////////////////////////

void delete_temp_directory(char* name) {
    struct Directory* current   = currentDirectory->subDirectory;
    struct Directory* prev      = NULL;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {

            if (current->files != NULL || current->subDirectory != NULL) {
                printf("\r\nDirectory not empty. Use cordell.\r\n");
                return;
            }

            if (prev == NULL) 
                currentDirectory = current->next;
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

void cordell_delete_temp_directory(char* name) {
    struct Directory* current   = currentDirectory->subDirectory;
    struct Directory* prev      = NULL;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            if (prev == NULL) 
                currentDirectory = current->next;
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

void delete_temp_file(char* name) {
    struct File* current    = currentDirectory->files;
    struct File* prev       = NULL;

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

void cordell_delete_temp_file(char* name) {
    struct File* current    = currentDirectory->files;
    struct File* prev       = NULL;

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

struct File* find_temp_file(char* name) {
    struct File* current = currentDirectory->files;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) 
            return current;
        
        current = current->next;
    }

    printf("\r\nFile not found.");
    return NULL;
}

struct Directory* find_temp_directory(char* name) {
    struct Directory* current = currentDirectory->subDirectory;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) 
            return current;

        current = current->next;
    }

    printf("\r\nDirectory not found.");
    return NULL;
}

void move_to_temp_directory(char* name) {
    struct Directory* neededDirectory = find_temp_directory(name);
    if (neededDirectory != NULL) 
        currentDirectory = neededDirectory;
}

void up_from_temp_directory() {
    if (currentDirectory->upDirectory != NULL)
        currentDirectory = currentDirectory->upDirectory;
}