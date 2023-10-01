#include <include/stdio.h>
#include <include/string.h>

#include "include/Keyboard.h"

#include "../../libs/core/shared/file_system/temp_file_system.h"

#define CORDELL_ACCESS      1
#define DEFAULT_ACCESS      0

/////////////////
//  CONSTANTS

    #define CORDELL_ATTENTION   "Non hai i permessi. Usa cordell."
    #define MAX_ATTEMPT_COUNT   4

//  CONSTANTS
/////////////////

void shell();
void execute_command(char* command, int access_level);