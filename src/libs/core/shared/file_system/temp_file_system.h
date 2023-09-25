#define MAX_DIRECTORIES 100 // Adjust the maximum number of directories as needed

typedef enum {
    NOT_ACCESSED        = 0,
    READABLE            = 1,
    READABLE_WRITABLE   = 2,
    EXETUABLE           = 3
} FileType;

typedef struct {
    FileType* fileType;

    char* name;
    char** content;

    struct File* next;
} File;

typedef struct {
    char* name;

    File* files;

    struct Directory* next;
} Directory;

void init_directory();
void create_temp_directory(char* name);
void create_temp_file(Directory* path, FileType* type, char* name, char** content);
void delete_temp_directory(char* name);

File* find_temp_file(Directory* directory, char* name);
Directory* find_temp_directory(char* name);

char** print_temp_dirs();
