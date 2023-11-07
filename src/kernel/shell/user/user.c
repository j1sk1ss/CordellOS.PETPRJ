#include "../include/user.h"

void init_users() {
    if (file_exist("users") == 1) 
        return;

    if (find_file("users")->fileType != 0)
        delete_file("users");

    // create default users file
    create_file(0, "users", USERS_SECTOR);
    write_file(find_file("users"), "admin[0[0\n");
}

struct User* login(char* user_name, char* pass) {
    struct User* user = NULL;
    char* data = read_file(find_file("users"));

    // Determine the number of lines (count the newline characters)
    int num_lines = 0;
    char* newline_pos = data;
    while (*newline_pos) {
        if (*newline_pos == '\n') 
            num_lines++;
        
        newline_pos++;
    }

    // Allocate an array of char* to store the lines
    char** lines = (char**)malloc(num_lines * sizeof(char*));

    // Split the data into lines and store them in the lines array
    char* raw_line = strtok(data, "\n");
    int line_index = 0;

    while (raw_line != NULL) {
        lines[line_index] = (char*)malloc(strlen(raw_line) + 2);

        strcat(lines[line_index], raw_line);
		strcat(lines[line_index], "\n");
    	strcat(lines[line_index], "\0");

        line_index++;
        raw_line = strtok(NULL, "\n");
    }

    int position = 0;
    while (position < num_lines) {
        char* token = strtok(lines[position], "[");
        if (token == NULL) 
            break;

        char* parsed_name = token;

        token = strtok(NULL, "[");
        if (token == NULL) 
            break;

        char* parsed_pass = atoi(token);

        token = strtok(NULL, "[");
        if (token == NULL) 
            break;
        
        int parsed_value = token;
        if (strcmp(parsed_name, user_name) == 0 && strcmp(parsed_pass, pass)) {
            user = (struct User*)malloc(sizeof(struct User));
            user->access = parsed_pass;

            break;
        }

        position++;
    }

    free(lines);
    free(data);

    return user;
}