#ifndef _KSHELL_H_
#define _KSHELL_H_


#include <stdlib.h>
#include <stddef.h>
#include <fatlib.h>
#include <string.h>
#include <bitmap.h>
#include <stdio.h>
#include <networking.h>
#include <keyboard.h>
#include <sysinfo.h>


//   ____  _   _ _____ _     _     
//  / ___|| | | | ____| |   | |    
//  \___ \| |_| |  _| | |   | |    
//   ___) |  _  | |___| |___| |___ 
//  |____/|_| |_|_____|_____|_____|
                                
/////////////////
//  CONSTANTS

    #define COMMAND_BUFFER  10
    #define COMMAND_LENGHT  128

    /////////////////
    //  COMMANDS

        #define COMMAND_HELP                            "aiuto"
        #define COMMAND_VERSION                         "ver"
        #define COMMAND_DISK_DATA                       "disco-dati"
        #define COMMAND_MEM_DATA                        "mem-dati"
        #define COMMAND_PAGE_DATA                       "page-data"
        #define COMMAND_CLEAR                           "pulito" 
        #define COMMAND_TIME                            "data"
        #define COMMAND_ECHO                            "eco"

        #define COMMAND_IN_DIR                          "cd"
        #define COMMAND_OUT_DIR                         ".."
        #define COMMAND_LIST_DIR                        "dir"

        #define COMMAND_IPCONFIG                        "ipconf"
        #define COMMAND_SEND_UDP_PACKET                 "udp-spacc"
        #define COMMAND_POP_UDP_PACKET                  "udp-ppacc"

        #define COMMAND_BMP_SHOW                        "guarda"
        #define COMMAND_CINFO                           "cinfo"
        #define COMMAND_FILE_VIEW                       "vista"
        #define COMMAND_FILE_RUN                        "fuga"

        #define COMMAND_EXIT                            "uscita"
        #define COMMAND_REBOOT                          "riavviare"

    //  COMMANDS
    /////////////////

//  CONSTANTS
/////////////////

void main(int argc, char* argv[]);
void shell_start_screen();
void execute_command(char* command);

#endif