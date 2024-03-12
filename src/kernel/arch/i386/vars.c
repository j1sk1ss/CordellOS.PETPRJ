#include "../../include/vars.h"


char* names_buffer[VARS_COUNT] = { NULL };
char* vars_buffer[VARS_COUNT]  = { NULL };


void VARS_init() {
    if (current_vfs->objexist("boot\\vars.txt") == 1) {
        // TODO: read vars from file
    }
    else {
        Content* vars = FAT_create_content("vars", FALSE, "txt");
        current_vfs->putobj("boot", vars);
        FSLIB_unload_content_system(vars);
    }
}

// -1 - nexists
// != -1 - exists
int VARS_exist(char* name) {
    for (int i = 0; i < VARS_COUNT; i++) 
        if (strstr(name, names_buffer[i]) == 0) return i;

    return -1;
}

char* VARS_get(char* name) {
    int position = VARS_exist(name);
    if (position == -1) return NULL;

    return vars_buffer[position];
}

void VARS_set(char* name, char* value) {
    int position = VARS_exist(name);
    if (position == -1) return;

    vars_buffer[position] = value;
}

void VARS_add(char* name, char* value) {
    int position = 0;
    for (position = 0; position < VARS_COUNT && names_buffer[position++] != NULL;);

    vars_buffer[position]  = value;
    names_buffer[position] = name;
}

void VARS_delete(char* name) {
    int position = VARS_exist(name);
    if (position == -1) return;

    free(names_buffer[position]);
    free(vars_buffer[position]);

    names_buffer[position] = NULL;
    vars_buffer[position]  = NULL;
}

void VARS_save() {
    // save file
}