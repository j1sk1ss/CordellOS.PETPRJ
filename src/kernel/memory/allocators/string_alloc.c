#include "string_alloc.h"

#include "../../io/string.h"
#include "memory/memory.h"

#include "../libs/core/allocators/first_fit/allocator.h"

char buffer[30];
char extendedBuffer[120];

char* allocate_string(char* string, size_t size) {
    if (size < 30) {
        strcpy(buffer, string);
        return buffer; 
    }

    if (size < 120) {
        strcpy(extendedBuffer, string);
        return extendedBuffer; 
    }

    // it shouldnt happend 
    char* eBuffer = malloc(size + 1); // +1 for the null terminator
    if (eBuffer == NULL) 
        return NULL;
    
    strcpy(eBuffer, string);

    return eBuffer;
}

void free_string() {
    // set zero state for string
    memset(buffer, 0, sizeof(buffer));
    memset(extendedBuffer, 0, sizeof(buffer));
}