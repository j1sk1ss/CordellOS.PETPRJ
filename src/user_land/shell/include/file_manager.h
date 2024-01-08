#pragma once

#include "../../../kernel/include/file_system.h"
#include "../../../kernel/include/fat.h"
#include "../../../kernel/include/keyboard.h"

#include "user.h"
#include "text_editor.h"


#define COLUMN_WIDTH 15

#define LINE        "+-----------------+-------------------+-----------+-----------+---------------+\n"
#define HEADER      "| Name            | Type              | Access    | Extension | Size          |\n"
#define UPPED_DIR   "|       ...       | None              | N/A       | N/A       | N/A           |\n"
#define EMPTY       "|                 |                   |           |           |               |\n"

#define EDIT_LINE   "| EDIT                                                                        |\n"
#define RENAME_LINE "| RENAME                                                                      |\n"
#define DELETE_LINE "| DELETE                                                                      |\n"

#define EDIT_POS        0
#define RENAME_POS      1
#define DELETE_POS      2


void open_file_manager(struct User* user);