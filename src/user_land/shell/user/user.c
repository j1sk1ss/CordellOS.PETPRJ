#include "../include/user.h"

void init_users() {
    if (FAT_content_exists("boot\\security") == 0) {
        struct FATContent* content = FAT_create_content("security", TRUE, NULL);
        FAT_put_content("boot", content);
        FAT_unload_content_system(content);
    }

    while (1) {
        if (FAT_content_exists("boot\\security\\users.txt") == 0) {
            struct FATContent* content = FAT_create_content("users", FALSE, "txt");
            FAT_put_content("boot\\security", content);
            FAT_unload_content_system(content);

            FAT_edit_content("boot\\security\\users.txt", "admin[000[0\nguest[666[123\n");
        }

        if (FAT_content_exists("boot\\security\\groups.txt") == 0) {
            struct FATContent* content = FAT_create_content("groups", FALSE, "txt");
            FAT_put_content("boot\\security", content);
            FAT_unload_content_system(content);

            FAT_edit_content("boot\\security\\groups.txt", "default[000[admin[guest\n");
        }

        return;
    }
}

//////////////////////////////////////////////////////////////////
//    ____ ____   ___  _   _ ____  
//   / ___|  _ \ / _ \| | | |  _ \ 
//  | |  _| |_) | | | | | | | |_) |
//  | |_| |  _ <| |_| | |_| |  __/ 
//   \____|_| \_\\___/ \___/|_|  

    struct Group* login_group(char* user_name) {
        char* groups = FAT_get_content("boot\\security\\groups.txt")->file->data;

        int num_lines = 0;
        char* newline_pos = groups;
        while (*newline_pos) {
            if (*newline_pos == '\n') 
                num_lines++;
            
            newline_pos++;
        }

        char** lines    = (char**)malloc(num_lines * sizeof(char*));
        char* raw_line  = strtok(groups, "\n");
        int line_index  = 0;

        while (raw_line != NULL) {
            lines[line_index] = (char*)malloc(strlen(raw_line) + 2);

            strcat(lines[line_index], raw_line);
            strcat(lines[line_index++], "\n\0");
            raw_line = strtok(NULL, "\n");
        }

        int position = 0; //default[000[admin[guest\n
        while (position < num_lines) {
            struct Group* group = malloc(sizeof(struct Group));

            char* token = strtok(lines[position], "[");
            char* parsed_name = token;

            group->name = (char*)malloc(strlen(parsed_name));
            strcpy(group->name, parsed_name);

            token = strtok(NULL, "[");
            char* parsed_access = token;

            group->read_access   = GUEST_ACCESS;
            group->write_access  = GUEST_ACCESS;
            group->edit_access   = GUEST_ACCESS;

            if (strlen(parsed_access) > 0)
                group->read_access = parsed_access[0] - '0';
            if (strlen(parsed_access) > 1)
                group->write_access = parsed_access[1] - '0';
            if (strlen(parsed_access) > 2)
                group->edit_access = parsed_access[2] - '0';

            while (token != NULL) {
                token = strtok(NULL, "[");
                if (token == NULL) 
                    break;

                char* parsed_user = token;
                if (strstr(parsed_user, user_name) == 0) return group;
            }

            free(group->name);
            free(group);

            free(lines[position++]);
        }

        free(lines);
        free(groups);

        return NULL;
    }

//
//
//////////////////////////////////////////////////////////////////
//   _   _ ____  _____ ____  
//  | | | / ___|| ____|  _ \ 
//  | | | \___ \|  _| | |_) |
//  | |_| |___) | |___|  _ < 
//   \___/|____/|_____|_| \_\

    struct User* login(char* user_name, char* pass, int all) {
        char* data = FAT_get_content("boot\\security\\users.txt")->file->data;

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
            strcat(lines[line_index++], "\n\0");
            raw_line = strtok(NULL, "\n");
        }

        int position = 0;
        while (position < num_lines) {
            char* token = strtok(lines[position], "[");
            char* parsed_name = token;

            token = strtok(NULL, "[");
            char* parsed_access = token;

            token = strtok(NULL, "[");
            char* parsed_password = token;

            //////
            //  NAME
            //
                users[position].name = (char*)malloc(strlen(parsed_name));
                strcpy(users[position].name, parsed_name);
            //
            //  NAME
            //////

            /////////////////////
            //
            //  ACCESS LEVELS
            //
                users[position].read_access   = GUEST_ACCESS;
                users[position].write_access  = GUEST_ACCESS;
                users[position].edit_access   = GUEST_ACCESS;
                users[position].users_count   = num_lines;

                if (strlen(parsed_access) > 0)
                    users[position].read_access = parsed_access[0] - '0';
                if (strlen(parsed_access) > 1)
                    users[position].write_access = parsed_access[1] - '0';
                if (strlen(parsed_access) > 2)
                    users[position].edit_access = parsed_access[2] - '0';


                /////////////////
                //  GROUP
                //
                    users[position].group = NULL;
                    struct Group* group = login_group(parsed_name);
                    if (group != NULL) {
                        users[position].read_access     = min(group->read_access, users[position].read_access);
                        users[position].write_access    = min(group->write_access, users[position].write_access);
                        users[position].edit_access     = min(group->edit_access, users[position].edit_access);

                        users[position].group = (char*)malloc(strlen(group->name));
                        strcpy(users[position].group, group->name);
                    }

                //
                //  GROUP
                /////////////////
                
            //
            //  ACCESS LEVELS
            //
            /////////////////////

            if (strstr(parsed_name, user_name) == 0 && strstr(parsed_password, pass) == 0 && all != 1) {
                free(lines[position]);
                free(lines);
                free(data); 

                return &users[position];
            }

            if (all != 1) {
                free(users[position].name);
                free(group->name);
                free(group);
            }

            free(lines[position++]);
        }

        free(lines);
        free(data);

        if (all == 1)
            return users;

        free(users);
        return NULL;
    }

//
//
//////////////////////////////////////////////////////////////////