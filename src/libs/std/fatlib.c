#include "../include/fatlib.h"

void FATLIB_unload_directories_system(struct UFATDirectory* directory) {
    struct UFATFile* current_file = directory->files;
    struct UFATFile* next_file    = NULL;
    while (current_file != NULL) {
        next_file = current_file->next;
        SYS_free(current_file);
        current_file = next_file;
    }

    if (directory->subDirectory != NULL)
        FATLIB_unload_directories_system(directory->subDirectory);

    if (directory->next != NULL) 
        FATLIB_unload_directories_system(directory->next);

    SYS_free(directory);
}

void FATLIB_unload_files_system(struct UFATFile* file) {
    struct UFATFile* current_file = file;
    struct UFATFile* next_file    = NULL;
    while (current_file != NULL) {
        next_file = current_file->next;
        SYS_free(current_file);
        current_file = next_file;
    }
}

char* FATLIB_change_path(const char* currentPath, const char* content) {
    if (content == NULL || content[0] == '\0') {
        const char* lastSeparator = strrchr(currentPath, '\\');
        if (lastSeparator == NULL) currentPath = "";
        else {
            size_t parentPathLen = lastSeparator - currentPath;
            char* parentPath = (char*)SYS_malloc(parentPathLen + 1);
            if (parentPath == NULL) {
                printf("Memory allocation failed\n");
                return NULL;
            }

            strncpy(parentPath, currentPath, parentPathLen);
            parentPath[parentPathLen] = '\0';

            free((void*)currentPath);
            currentPath = parentPath;
        }
    } else {
        size_t newPathLen = strlen(currentPath) + strlen(content) + 2;
        char* newPath = (char*)SYS_malloc(newPathLen);
        if (newPath == NULL) {
            printf("Memory allocation failed\n");
            return NULL;
        }

        strcpy(newPath, currentPath);
        if (newPath[strlen(newPath) - 1] != '\\') 
            strcat(newPath, "\\");
        
        strcat(newPath, content);

        SYS_free((void*)currentPath);
        currentPath = newPath;
    }

    return strdup(currentPath);
}