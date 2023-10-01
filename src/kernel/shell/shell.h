#include <include/stdio.h>
#include <include/string.h>

#include "include/Keyboard.h"

#include "../../libs/core/shared/file_system/temp_file_system.h"

#define CORDELL_ACCESS      1
#define DEFAULT_ACCESS      0

void shell();
void execute_command(char* command, int access_level);