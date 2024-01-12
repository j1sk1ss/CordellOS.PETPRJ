#pragma once

#include "../../../kernel/include/fat.h"
#include "../../../kernel/include/keyboard.h"

#include "user.h"
#include "text_editor.h"


#define ROWS            14
#define LINE_OFFSEET    4

#define LINE        "+-------------+--------+----------+----------+-----------+------+-------------+\n"
#define HEADER      "| NAME        | TYPE   | CREATED  | MODIF    | ACCESS    | EXT  | SIZE        |\n"
#define UPPED_DIR   "|     ...     | NONE   | N/A      | N/A      | N/A       | N/A  | N/A         |\n"
#define EMPTY       "|             |        |          |          |           |      |             |\n"

#define EDIT_LINE   "| EDIT                                                                        |\n"
#define VIEW_LINE   "| VIEW                                                                        |\n"
#define RENAME_LINE "| RENAME                                                                      |\n"
#define DELETE_LINE "| DELETE                                                                      |\n"

#define EDIT_POS        0
#define VIEW_POS        1
#define RENAME_POS      2
#define DELETE_POS      3


void open_file_manager(struct User* user);