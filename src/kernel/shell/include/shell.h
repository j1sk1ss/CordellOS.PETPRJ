#include <include/stdio.h>
#include <include/string.h>
#include <include/date_time.h>
#include <include/file_system.h>
#include <include/ata.h>
#include <include/util.h>
#include <include/asm_compiler.h>

#include "keyboard.h"
#include "user.h"
#include "file_manager.h"
#include "snake.h"

#include "../libs/core/shared/include/x86.h"

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
        #define COMMAND_CLEAR                           "clear" 
        #define COMMAND_TIME                            "date"
        #define COMMAND_ECHO                            "eco"
        #define COMMAND_USERS                           "users"

        #define COMMAND_GET_HDD_SECTOR                  "rsector"
        #define COMMAND_SET_HDD_SECTOR                  "wsector"
        #define COMMAND_CLEAR_SECTOR                    "clsector"

        #define COMMAND_CREATE_DIR                      "mkdir"
        #define COMMAND_DELETE_DIR                      "rmdir"

        #define COMMAND_IN_DIR                          "cd"
        #define COMMAND_OUT_DIR                         ".."
        #define COMMAND_LIST_DIR                        "dir"

        #define COMMAND_FILE_VIEW                       "vista"
        #define COMMAND_FILE_EDIT                       "modif"
        #define COMMAND_FILE_RUN                        "fuga"
        #define COMMAND_FILE_ASM_RUN                    "asm"

        #define COMMAND_GO_TO_MANAGER                   "mng"

        #define COMMAND_CREATE_FILE                     "mkfile"
        #define COMMAND_DELETE_FILE                     "rmfile"

        #define COMMAND_CALCULATOR                      "calc"
        #define COMMAND_SNAKE_GAME                      "snake"
        #define COMMAND_SPLIT_LINE                      "diviso"

    //  COMMANDS
    /////////////////

//  CONSTANTS
/////////////////


void shell();
void execute_command(char* command, int access_level);