#include "../string/string.h"
#include "../allocator/allocator.h"

#include "temp_file_system.h"


Directory* directory; // Array to store directory pointers

void init_directory() {
    directory = NULL; // Initialize your directory data structure
}

void create_temp_directory(char* name) {
    Directory* newDirectory = (Directory*)pmalloc(sizeof(Directory));
    memset(newDirectory, 0, sizeof(newDirectory));

    newDirectory->name  = name;
    newDirectory->files = NULL;
    newDirectory->next  = NULL;

    if (directory == NULL)
        directory = newDirectory;
    else {
        Directory* current = directory;
        while (current->next != NULL) 
            current = current->next;

        current->next = newDirectory;
    }
}

void create_temp_file(Directory* path, FileType* type, char* name, char** content) {
    File* newFile = (File*)pmalloc(sizeof(File));
    memset(newFile, 0, sizeof(newFile));

    newFile->fileType   = type;
    newFile->name       = name;
    newFile->content    = content;
    newFile->next       = path->files;

    path->files = newFile;
}

void delete_temp_directory(char* name) {
    Directory* current = directory;
    Directory* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            if (prev == NULL) 
                directory = current->next;
            else 
                prev->next = current->next;
            
            free(current);
            break;
        }

        prev    = current;
        current = current->next;
    }
}

void delete_temp_file(Directory* path, char* name) {
    File* current = path->files;
    File* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            if (prev == NULL) 
                path->files = current->next;
            else 
                prev->next = current->next;
            
            free(current);
            break;
        }

        prev    = current;
        current = current->next;
    }
}

File* find_temp_file(Directory* directory, char* name) {
    File* current = directory->files;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) 
            return current;
        
        current = current->next;
    }

    return NULL; // File not found
}

Directory* find_temp_directory(char* name) {
    Directory* current = directory;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) 
            return current;

        current = current->next;
    }

    return NULL; // Directory not found
}

char** print_temp_dirs() {
    Directory* current = directory;
    int count = 0;
    
    // Count the number of directories
    while (current != NULL) {
        count++;
        current = current->next;
    }
    
    char* dirNames[100];
    current = directory;
    
    // Store directory names in the array
    for (int i = 0; i < count; i++) {
        dirNames[i] = current->name;
        current     = current->next;
    }
    
    return dirNames;
}