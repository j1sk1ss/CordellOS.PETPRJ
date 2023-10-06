#include <include/stdio.h>
#include <include/string.h>

#include "Keyboard.h"

#include "../libs/core/shared/file_system/temp_file_system.h"
#include "../libs/core/shared/include/x86.h"

#define CORDELL_ACCESS      1
#define DEFAULT_ACCESS      0

/////////////////
//  CONSTANTS

    #define CORDELL_ATTENTION   "Non hai i permessi. Usa cordell."
    #define MAX_ATTEMPT_COUNT   4

    /////////////////
    //  COMMANDS

        #define COMMAND_CLEAR                           "clear" 
        #define COMMAND_ECHO                            "eco"
        #define COMMAND_PASS                            "setpas"

        #define COMMAND_CREATE_DIR                      "mkdir"
        #define COMMAND_DELETE_DIR                      "rmdir"

        #define COMMAND_IN_DIR                          "cd"
        #define COMMAND_OUT_DIR                         ".."
        #define COMMAND_LIST_DIR                        "dir"

        #define COMMAND_FILE_VIEW                       "view"
        #define COMMAND_FILE_EDIT                       "edit"
        #define COMMAND_FILE_RUN                        "run"

        #define COMMAND_CREATE_FILE                     "mkfile"
        #define COMMAND_DELETE_FILE                     "rmfile"

        #define COMMAND_CALCULATOR                      "calc"

    //  COMMANDS
    /////////////////

//  CONSTANTS
/////////////////

void shell();
void execute_command(char* command, int access_level);