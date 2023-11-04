#include <include/file_system.h>

struct User {
    int access;
};

void init_users();
struct User* login(char* user_name, char* pass);
