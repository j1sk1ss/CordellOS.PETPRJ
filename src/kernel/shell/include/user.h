#pragma once

#include <include/file_system.h>

struct User {
    char* name;
    
    int read_access;
    int write_access;
    int edit_access;

    int users_count;
};

void init_users();
struct User* login(char* user_name, char* pass, int all);
void print_users_table();
