#include "include/user_land.h"

#include "shell/include/shell.h"

void user_land_entry(void) {
    shell();
    return -1;
}