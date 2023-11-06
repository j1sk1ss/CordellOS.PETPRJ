#pragma once

#include <include/file_system.h>

#include "Keyboard.h"


#define COLUMN_WIDTH 15

#define LINE        "+-----------------+-------------------+-----------+---------------------------+\n"
#define HEADER      "| Name            | Type              | Access    | Size                      |\n"
#define UPPED_DIR   "|       ...       | None              | N/A       | N/A                       |\n"
#define EMPTY       "|                 |                   |           |                           |\n"

#define EDIT_LINE   "| EDIT                                                                        |\n"
#define READ_LINE   "| VIEW                                                                        |\n"
#define RUN_LINE    "| RUN SCRIPT                                                                  |\n"
#define ASM_LINE    "| RUN ASM                                                                     |\n"
#define DELETE_LINE "| DELETE                                                                      |\n"

#define READ_POS        0
#define EDIT_POS        1
#define ASM_POS         2
#define DELETE_POS      3

void open_file_manager(int access_level);