#include "../include/user.h"

void init_users() {
    if (file_exist("users") == 1) 
        return;

    struct File* file = find_file("users");
    if (file->read_level != 0 || file->write_level != 0 || file->edit_level != 0)
        delete_file("users");

    // create default users file
    create_file(0, 0, 0, "users", USERS_SECTOR);
    write_file(find_file("users"), "admin[000[0\n");
}


struct User* login(char* user_name, char* pass, int all) {
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
    struct User* users = malloc(num_lines * sizeof(struct User));

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

        char* parsed_pass = token;
        token = strtok(NULL, "[");
        if (token == NULL) 
            break;
        
        char* parsed_value = token;

        users[position].name = (char*)malloc(strlen(parsed_name));
        strcpy(users[position].name, parsed_name);

        users[position].read_access   = 6;
        users[position].write_access  = 6;
        users[position].edit_access   = 6;
        users[position].users_count   = num_lines;

        if (strlen(parsed_pass) > 0)
            users[position].read_access = parsed_pass[0] - '0';
        if (strlen(parsed_pass) > 1)
            users[position].write_access = parsed_pass[1] - '0';
        if (strlen(parsed_pass) > 2)
            users[position].edit_access = parsed_pass[2] - '0';

        if (strstr(parsed_name, user_name) == 0 && strstr(parsed_value, pass) == 0 && all != 1) {
            free(lines[position]);
            free(lines);
            free(data);            
            return &users[position];
        }
            
        free(parsed_name);
        free(parsed_pass);
        free(parsed_value);

        if (all != 1) {
            free(users[position].name);
            free(users[position]);
        }

        position++;
    }

    free(lines[position]);
    free(lines);
    free(data);

    if (all == 1)
        return users;

    free(users);
    return NULL;
}

void print_users_table() {
    struct User* users = login("", "", 1);
    int size = users[0].users_count;

    printf("\n");
    printf(" ________________________________________________\n");
    printf("|     NAME     |   READ   |   WRITE   |   EDIT   |\n");
    printf("|--------------|----------|-----------|----------|\n");

    for (int i = 0; i < size; i++) {
        char name[13];
        memset(name, ' ', 12);
        name[12] = '\0';

        int name_length = strlen(users[i].name);
        if (name_length <= 12) strncpy(name, users[i].name, name_length);
        else strncpy(name, users[i].name, 9);   

        printf("| %s |   %i      |   %i       |   %i      |\n", name, users[i].read_access, users[i].write_access, users[i].edit_access);     
    }

    printf("|________________________________________________|\n");
    free(users);
} 