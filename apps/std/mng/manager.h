#include <fatlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <keyboard.h>


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


void main(int argc, char* argv[]);
void keyboard_wait(char symbol);
char* convert_date(int day, int month, int year);
int set_line_color(int line, uint8_t color);
void open_file_manager(char* path);
void execute_item(char action_type);
void print_directory_data();