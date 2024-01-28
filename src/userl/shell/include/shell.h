#include "../../../libs/include/stdio.h"
#include "../../../libs/include/string.h"
#include "../../../libs/include/fatlib.h"
#include "../../../libs/include/stdlib.h"

#include "user.h"
#include "file_manager.h"
#include "text_editor.h"
#include "asm_compiler.h"
#include "calculator.h"


#define SUPER_DERICTIVE        0
#define CORDELL_DERICTIVE      1
#define DEFAULT_DERICTIVE      2

/////////////////
//  CONSTANTS

    #define CORDELL_ATTENTION   "\nNon hai i permessi. Usa cordell."
    #define GUEST_ATTENTION     "\nTu in modalita' ospite. Azione respinta."
    #define MAX_ATTEMPT_COUNT   4

    /////////////////
    //  COMMANDS

        #define COMMAND_HELP                            "aiuto"
        #define COMMAND_VERSION                         "ver"
        #define COMMAND_DISK_DATA                       "disk-data"
        #define COMMAND_CLEAR                           "clear" 
        #define COMMAND_TIME                            "date"
        #define COMMAND_ECHO                            "eco"
        #define COMMAND_USERS                           "users"

        #define COMMAND_CREATE_DIR                      "mkdir"
        #define COMMAND_CREATE_FILE                     "mkfile"

        #define COMMAND_IN_DIR                          "cd"
        #define COMMAND_OUT_DIR                         ".."
        #define COMMAND_LIST_DIR                        "dir"

        #define COMMAND_FILE_VIEW                       "vista"
        #define COMMAND_FILE_EDIT                       "modif"
        #define COMMAND_FILE_RUN                        "fuga"
        #define COMMAND_FILE_ASM_RUN                    "asm"

        #define COMMAND_GO_TO_MANAGER                   "mng"

        #define COMMAND_DELETE_CONTENT                  "rmcontent"

        #define COMMAND_CALCULATOR                      "calc"
        #define COMMAND_SNAKE_GAME                      "snake"
        #define COMMAND_TETRIS_GAME                     "tetris"

    //  COMMANDS
    /////////////////

//  CONSTANTS
/////////////////


void shell();
void execute_command(char* command, int cordell_derictive, char* path, struct User* user);