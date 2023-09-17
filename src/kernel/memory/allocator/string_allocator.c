#include "string_allocator.h"

#include "../../io/string.h"
#include "../../io/stdio.h"

#include "memory/memory.h"

#include "allocator.h"

char* allocate_string(char* string, size_t size) {
    if (size < 30) {
        static char standartBuffer[31];
        strcpy(standartBuffer, string);

        return standartBuffer;
    }

    char* buffer = (char*)malloc(size);
    if (buffer == NULL)
        return NULL;

    memset(buffer, 0, size);
    strcpy(buffer, string);

    printf("CODE: [%s]", string);

    return buffer; 
    
}

void free_string(char* string) {
    // set zero state for string
    free(string);
}