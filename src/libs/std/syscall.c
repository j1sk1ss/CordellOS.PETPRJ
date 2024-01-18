#include "../include/syscall.h"

/////////////////////////////
//   ____  _   _ _____ ____  
//  |  _ \| | | |_   _/ ___| 
//  | |_) | | | | | | \___ \ 
//  |  __/| |_| | | |  ___) |
//  |_|    \___/  |_| |____/ 
//
//  Puts a string in screen (used kernel printf commands)
//
    void SYS_puts(char* string) {
        __asm__ volatile(
            "movl $0, %%eax\n"
            "movl $1, %%ebx\n"
            "movl %0, %%ecx\n"
            "int %1\n"
            :
            : "r"(string), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//   ____  _   _ _____ ____ 
//  |  _ \| | | |_   _/ ___|
//  | |_) | | | | | || |    
//  |  __/| |_| | | || |___ 
//  |_|    \___/  |_| \____|
//
//  Puts a char in screen (used kernel printf commands)
//  ECX - character
//
    void SYS_putc(char character) {
        __asm__ volatile(
            "movl $1, %%eax\n"
            "movl $1, %%ebx\n"
            "movl %0, %%ecx\n"
            "movl $1, %%edx\n"
            "int %1\n"
            :
            : "r"((int)character), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//    ____ ____  _   _ _____ ____ 
//   / ___|  _ \| | | |_   _/ ___|
//  | |   | |_) | | | | | || |    
//  | |___|  __/| |_| | | || |___ 
//   \____|_|    \___/  |_| \____|
//
//  Puts a char in screen with color (used kernel printf commands)
//  ECX - color
//  EBX - character
//
    void SYS_cputc(uint8_t color, char character) {
        __asm__ volatile(
            "movl $13, %%eax\n"
            "movl %1, %%ebx\n"
            "movl %0, %%ecx\n"
            "int %2\n"
            :
            : "r"((int)color), "r"((int)character), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//   ____   ____ ____   ____ _     ____  
//  / ___| / ___|  _ \ / ___| |   |  _ \ 
//  \___ \| |   | |_) | |   | |   | |_) |
//   ___) | |___|  _ <| |___| |___|  _ < 
//  |____/ \____|_| \_\\____|_____|_| \_\
//
//  Fill screen by selected color (Not VBE color)
//  ECX - color
//
void SYS_scrclr(uint8_t color) {
    __asm__ volatile(
        "movl $14, %%eax\n"
        "movl $1, %%ebx\n"
        "movl %0, %%ecx\n"
        "movl $1, %%edx\n"
        "int %1\n"
        :
        : "r"((int)color), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );
}
//
/////////////////////////////

/////////////////////////////
//    ____ _     ____  ____  
//   / ___| |   |  _ \/ ___| 
//  | |   | |   | |_) \___ \ 
//  | |___| |___|  _ < ___) |
//   \____|_____|_| \_\____/ 
//
//  Clear entire screen (used kernel printf commands)
//
    void SYS_clrs() {
        __asm__ volatile(
            "movl $2, %%eax\n"
            "movl $1, %%ebx\n"
            "movl $1, %%edx\n"
            "int %0\n"
            :
            : "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//   ____  _     _____ _____ ____  
//  / ___|| |   | ____| ____|  _ \ 
//  \___ \| |   |  _| |  _| | |_) |
//   ___) | |___| |___| |___|  __/ 
//  |____/|_____|_____|_____|_|   
//
//  Sleep function
//  EDX - time
//
    void SYS_sleep(int milliseconds) {
        __asm__ volatile(
            "movl $3, %%eax\n"
            "movl $1, %%ebx\n"
            "movl %0, %%edx\n"
            "int %1\n"
            :
            : "r"(milliseconds), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//   _  _________   ______   ___    _    ____  ____   __        ___    ___ _____ 
//  | |/ / ____\ \ / / __ ) / _ \  / \  |  _ \|  _ \  \ \      / / \  |_ _|_   _|
//  | ' /|  _|  \ V /|  _ \| | | |/ _ \ | |_) | | | |  \ \ /\ / / _ \  | |  | |  
//  | . \| |___  | | | |_) | |_| / ___ \|  _ <| |_| |   \ V  V / ___ \ | |  | |  
//  |_|\_\_____| |_| |____/ \___/_/   \_\_| \_\____/     \_/\_/_/   \_\___| |_|  
//
//  This function waits an any button pressing from user
//  ECX - pointer to character
//
    char SYS_keyboard_wait_key() {
        char key;
        __asm__ volatile(
            "movl $4, %%eax\n"
            "movl $1, %%ebx\n"
            "movl %0, %%ecx\n"
            "movl $0, %%edx\n"
            "int %1\n"
            :
            : "r"(&key), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );

        return key;
    }
//
/////////////////////////////

/////////////////////////////
//   _  _________   ______   ___    _    ____  ____     ____ _____ _____ 
//  | |/ / ____\ \ / / __ ) / _ \  / \  |  _ \|  _ \   / ___| ____|_   _|
//  | ' /|  _|  \ V /|  _ \| | | |/ _ \ | |_) | | | | | |  _|  _|   | |  
//  | . \| |___  | | | |_) | |_| / ___ \|  _ <| |_| | | |_| | |___  | |  
//  |_|\_\_____| |_| |____/ \___/_/   \_\_| \_\____/   \____|_____| |_| 
//
//  This function just get key from keyboard port (not wait for pressing)
//  ECX - pointer to character
//
    char SYS_keyboard_get_key() {
        char key;
        __asm__ volatile(
            "movl $5, %%eax\n"
            "movl $1, %%ebx\n"
            "movl %0, %%ecx\n"
            "movl $1, %%edx\n"
            "int %1\n"
            :
            : "r"(&key), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );

        return key;
    }
//
/////////////////////////////

/////////////////////////////
//   _  _________   ______   ___    _    ____  ____    ____  _____    _    ____  
//  | |/ / ____\ \ / / __ ) / _ \  / \  |  _ \|  _ \  |  _ \| ____|  / \  |  _ \ 
//  | ' /|  _|  \ V /|  _ \| | | |/ _ \ | |_) | | | | | |_) |  _|   / _ \ | | | |
//  | . \| |___  | | | |_) | |_| / ___ \|  _ <| |_| | |  _ <| |___ / ___ \| |_| |
//  |_|\_\_____| |_| |____/ \___/_/   \_\_| \_\____/  |_| \_\_____/_/   \_\____/ 
//
//  This function reads keyboard input from user until he press ENTER that returns string of input
//  EAX - interrupt / buffer
//  EDX - color
//  EBX - mode
//
    char* SYS_keyboard_read(int mode, uint8_t color) {
        char* buffer;
        __asm__ volatile(
            "movl $19, %%eax\n"
            "movl %2, %%ebx\n"
            "movl %1, %%edx\n"
            "int %3\n"
            "movl %%eax, %0\n"
            : "=r"(buffer)
            : "r"((int)color), "r"(mode), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );

        return buffer;
    }
//
/////////////////////////////

/////////////////////////////
//   ____    _  _____ _____ _____ ___ __  __ _____ 
//  |  _ \  / \|_   _| ____|_   _|_ _|  \/  | ____|
//  | | | |/ _ \ | | |  _|   | |  | || |\/| |  _|  
//  | |_| / ___ \| | | |___  | |  | || |  | | |___ 
//  |____/_/   \_\_| |_____| |_| |___|_|  |_|_____|
//
//  Return date time from cmos in short*
//  ECX - pointer to array
//
//  data[0] - seconds
//  data[1] - minute
//  data[2] - hour
//  data[3] - day
//  data[4] - month
//  data[5] - year
//
    void SYS_get_datetime(short* data) {
        __asm__ volatile(
            "movl $6, %%eax\n"
            "movl $1, %%ebx\n"
            "movl %0, %%ecx\n"
            "movl $1, %%edx\n"
            "int %1\n"
            :
            : "r"(data), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//   __  __    _    _     _     ___   ____ 
//  |  \/  |  / \  | |   | |   / _ \ / ___|
//  | |\/| | / _ \ | |   | |  | | | | |    
//  | |  | |/ ___ \| |___| |__| |_| | |___ 
//  |_|  |_/_/   \_\_____|_____\___/ \____|
//
//  Allocate memory and return pointer
//  EBX - size
//  EAX - returned pointer to allocated memory
//
    void* SYS_malloc(uint32_t size) {
        void* allocated_memory;
        __asm__ volatile(
            "movl $7, %%eax\n"
            "movl %1, %%ebx\n"
            "int $0x80\n"
            "movl %%eax, %0\n"
            : "=r" (allocated_memory)
            : "r" (size)
            : "%eax", "%ebx"
        );

        return allocated_memory;
    }
//
/////////////////////////////

/////////////////////////////
//   _____ ____  _____ _____ 
//  |  ___|  _ \| ____| ____|
//  | |_  | |_) |  _| |  _|  
//  |  _| |  _ <| |___| |___ 
//  |_|   |_| \_\_____|_____|
//
//  Free alocated memory
//  EBX - pointer to allocated memory
//
    void SYS_free(void* ptr) {
        __asm__ volatile(
            "movl $8, %%eax\n"
            "movl %0, %%ebx\n"
            "int $0x80\n"
            :
            : "r" (ptr)
            : "%eax", "%ebx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//   ____  _____    _    ____    _____ ___ _     _____ 
//  |  _ \| ____|  / \  |  _ \  |  ___|_ _| |   | ____|
//  | |_) |  _|   / _ \ | | | | | |_   | || |   |  _|  
//  |  _ <| |___ / ___ \| |_| | |  _|  | || |___| |___ 
//  |_| \_\_____/_/   \_\____/  |_|   |___|_____|_____|
//
//  Read file content by path
//  ECX - path
//  EAX - returned data
//             
    char* SYS_fread(const char* path) {
        void* pointed_data;
        uint32_t file_path_ptr = (uint32_t)path;

        __asm__ volatile(
            "movl $9, %%eax\n"
            "movl %1, %%ebx\n"
            "movl $0, %%ecx\n"
            "int %2\n"
            "movl %%eax, %0\n"
            : "=r"(pointed_data)
            : "r"(file_path_ptr), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );

        return pointed_data;
    }
//
/////////////////////////////

/////////////////////////////
//  __        ______  ___ _____ _____   _____ ___ _     _____ 
//  \ \      / /  _ \|_ _|_   _| ____| |  ___|_ _| |   | ____|
//   \ \ /\ / /| |_) || |  | | |  _|   | |_   | || |   |  _|  
//    \ V  V / |  _ < | |  | | | |___  |  _|  | || |___| |___ 
//     \_/\_/  |_| \_\___| |_| |_____| |_|   |___|_____|_____|
//
//  Write file to content (if it exists) by path (rewrite all content)
//  EBX - path
//  ECX - data
//
    void SYS_fwrite(const char* path, const char* data) {
        uint32_t file_path_ptr = (uint32_t)path;
        uint32_t data_ptr = (uint32_t)data;

        __asm__ volatile(
            "movl $10, %%eax\n"
            "movl %0, %%ebx\n"
            "movl %1, %%ecx\n"
            "int %2\n"
            :
            : "r"(file_path_ptr), "r"(data_ptr), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//    ___  ____  _____ _   _ ____ ___ ____  
//   / _ \|  _ \| ____| \ | |  _ \_ _|  _ \ 
//  | | | | |_) |  _| |  \| | | | | || |_) |
//  | |_| |  __/| |___| |\  | |_| | ||  _ < 
//   \___/|_|   |_____|_| \_|____/___|_| \_\
//
//  Returns linked list of dir content by path
//  EBX - path
//  ECX - pointer to directory
//
    struct UFATDirectory* SYS_opendir(const char* path) {
        uint32_t path_ptr = (uint32_t)path;
        struct UFATDirectory* directory = (struct UFATDirectory*)SYS_malloc(sizeof(struct UFATDirectory));
        __asm__ volatile(
            "movl $11, %%eax\n"
            "movl %0, %%ebx\n"
            "movl %1, %%ecx\n"
            "int %2\n"
            :
            : "r"(path_ptr), "r"(directory), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx"
        );

        return directory;
    }
//
/////////////////////////////

/////////////////////////////
//    ____ ___  _   _ _____ _____ _   _ _____   _______  _____ ____ _____ ____  
//   / ___/ _ \| \ | |_   _| ____| \ | |_   _| | ____\ \/ /_ _/ ___|_   _/ ___| 
//  | |  | | | |  \| | | | |  _| |  \| | | |   |  _|  \  / | |\___ \ | | \___ \ 
//  | |__| |_| | |\  | | | | |___| |\  | | |   | |___ /  \ | | ___) || |  ___) |
//   \____\___/|_| \_| |_| |_____|_| \_| |_|   |_____/_/\_\___|____/ |_| |____/ 
//
//  Function for checking if content exist by this path
//  EBX - path
//  ECX - result (0 - nexists)
//
    int SYS_cexists(const char* path) {
        int result;
        uint32_t path_ptr = (uint32_t)path;

        __asm__ volatile(
            "movl $15, %%eax\n"
            "movl %0, %%ebx\n"
            "movl %1, %%ecx\n"
            "int %2\n"
            : 
            : "r"(path_ptr), "r"(&result), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx"
        );

        return result;
    }
//
/////////////////////////////

/////////////////////////////
//   __  __ _  _______ ___ _     _____ 
//  |  \/  | |/ /  ___|_ _| |   | ____|
//  | |\/| | ' /| |_   | || |   |  _|  
//  | |  | | . \|  _|  | || |___| |___ 
//  |_|  |_|_|\_\_|   |___|_____|_____|
//
//  This function creates file
//  EBX - path
//  RCX - name (with extention)
//
    void SYS_mkfile(const char* path, const char* name) {
        uint32_t file_path_ptr = (uint32_t)path;
        uint32_t file_name_ptr = (uint32_t)name;

        __asm__ volatile(
            "movl $16, %%eax\n"
            "movl %0, %%ebx\n"
            "movl %1, %%ecx\n"
            "int %2\n"
            :
            : "r"(file_path_ptr), "r"(file_name_ptr), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//   __  __ _  ______ ___ ____  
//  |  \/  | |/ /  _ \_ _|  _ \ 
//  | |\/| | ' /| | | | || |_) |
//  | |  | | . \| |_| | ||  _ < 
//  |_|  |_|_|\_\____/___|_| \_\
//
//  This function creates directory
//  EBX - path
//  ECX - name
//
    void SYS_mkdir(const char* path, const char* name) {
        uint32_t dir_path_ptr = (uint32_t)path;
        uint32_t dir_name_ptr = (uint32_t)name;

        __asm__ volatile(
            "movl $17, %%eax\n"
            "movl %0, %%ebx\n"
            "movl %1, %%ecx\n"
            "int %2\n"
            :
            : "r"(dir_path_ptr), "r"(dir_name_ptr), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//   ____  _____ __  __  _____     _______    ____ ___  _   _ _____ _____ _   _ _____ 
//  |  _ \| ____|  \/  |/ _ \ \   / / ____|  / ___/ _ \| \ | |_   _| ____| \ | |_   _|
//  | |_) |  _| | |\/| | | | \ \ / /|  _|   | |  | | | |  \| | | | |  _| |  \| | | |  
//  |  _ <| |___| |  | | |_| |\ V / | |___  | |__| |_| | |\  | | | | |___| |\  | | |  
//  |_| \_\_____|_|  |_|\___/  \_/  |_____|  \____\___/|_| \_| |_| |_____|_| \_| |_| 
//
//  This function remove content
//  EBX - path
//  ECX - name (if file - with extention)
//
    void SYS_rmcontent(const char* path, const char* name) {
        uint32_t dir_path_ptr = (uint32_t)path;
        uint32_t content_name_ptr = (uint32_t)name;

        __asm__ volatile(
            "movl $18, %%eax\n"
            "movl %0, %%ebx\n"
            "movl %1, %%ecx\n"
            "int %2\n"
            :
            : "r"(dir_path_ptr), "r"(content_name_ptr), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx"
        );
    }
//
/////////////////////////////

void SYS_chgcontent(const char* path, struct udirectory_entry* meta) {
    uint32_t dir_path_ptr = (uint32_t)path;
    uint32_t content_name_ptr = (uint32_t)meta;

    __asm__ volatile(
        "movl $25, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "int %2\n"
        :
        : "r"(dir_path_ptr), "r"(content_name_ptr), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx"
    );
}

/////////////////////////////
//   _______  _______ ____   _____ ___ _     _____ 
//  | ____\ \/ / ____/ ___| |  ___|_ _| |   | ____|
//  |  _|  \  /|  _|| |     | |_   | || |   |  _|  
//  | |___ /  \| |__| |___  |  _|  | || |___| |___ 
//  |_____/_/\_\_____\____| |_|   |___|_____|_____|
//
//  Function that executes ELF file
//  EAX - result CODE
//  EBX - path
//  ECX - args (count)
//  EDX - argv (array of args)
//
    int SYS_fexec(char* path, int args, char** argv) {
        int result = 0;
        uint32_t file_path_ptr = (uint32_t)path;

        __asm__ volatile(
            "movl $12, %%eax\n"
            "movl %1, %%ebx\n"
            "movl %2, %%ecx\n"
            "movl %3, %%edx\n"
            "int %4\n"
            "movl %%eax, %0\n"
            : "=r"(result)
            : "m"(file_path_ptr), "r"(args), "m"(argv), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );

        return result;
    }
//
/////////////////////////////

/////////////////////////////
//   ____   ____ ____  _____ _____ _   _    ____ _   _    _    ____  
//  / ___| / ___|  _ \| ____| ____| \ | |  / ___| | | |  / \  |  _ \ 
//  \___ \| |   | |_) |  _| |  _| |  \| | | |   | |_| | / _ \ | |_) |
//   ___) | |___|  _ <| |___| |___| |\  | | |___|  _  |/ ___ \|  _ < 
//  |____/ \____|_| \_\_____|_____|_| \_|  \____|_| |_/_/   \_\_| \_\
//
//  Get char from screen by coordinates
//  AL - character
//  EBX - x
//  ECX - y
//
    char SYS_scrchar(int x, int y) {
        char result;
        __asm__ volatile(
            "movl $22, %%eax\n"
            "movl %1, %%ebx\n"
            "movl %2, %%ecx\n"
            "int %3\n"
            "movb %%al, %0\n"
            : "=r"(result)
            : "r"(x), "r"(y), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx"
        );

        return result;
    }
//
/////////////////////////////

/////////////////////////////
//    ____ _____ _____    ____ _   _ ____  ____   ___  ____  
//   / ___| ____|_   _|  / ___| | | |  _ \/ ___| / _ \|  _ \ 
//  | |  _|  _|   | |   | |   | | | | |_) \___ \| | | | |_) |
//  | |_| | |___  | |   | |___| |_| |  _ < ___) | |_| |  _ < 
//   \____|_____| |_|    \____|\___/|_| \_\____/ \___/|_| \_\
//
//  Function that gets cursor coordinates
//  ECX - array of coordinates
//  
//  data[0] - x
//  data[1] - y
//
    int* SYS_get_cursor() {
        int result[2];
        __asm__ volatile(
            "movl $21, %%eax\n"
            "movl $1, %%ebx\n"
            "movl %0, %%ecx\n"
            "int %1\n"
            :
            : "r"(&result), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx", "edx"
        );

        return result;
    }
//
/////////////////////////////

/////////////////////////////
//   ____  _____ _____    ____ _   _ ____  ____   ___  ____  
//  / ___|| ____|_   _|  / ___| | | |  _ \/ ___| / _ \|  _ \ 
//  \___ \|  _|   | |   | |   | | | | |_) \___ \| | | | |_) |
//   ___) | |___  | |   | |___| |_| |  _ < ___) | |_| |  _ < 
//  |____/|_____| |_|    \____|\___/|_| \_\____/ \___/|_| \_\
//
//  Function set cursor by coordinates
//  EBX - x
//  ECX - y
//
    void SYS_set_cursor(int x, int y) {
        __asm__ volatile(
            "movl $20, %%eax\n"
            "movl %0, %%ebx\n"
            "movl %1, %%ecx\n"
            "int %2\n"
            :
            : "r"(x), "r"(y), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx"
        );
    }
//
/////////////////////////////

/////////////////////////////
//   ____  _____ _____   ____   ____ ____  _____ _____ _   _    ____ _   _    _    ____  
//  / ___|| ____|_   _| / ___| / ___|  _ \| ____| ____| \ | |  / ___| | | |  / \  |  _ \ 
//  \___ \|  _|   | |   \___ \| |   | |_) |  _| |  _| |  \| | | |   | |_| | / _ \ | |_) |
//   ___) | |___  | |    ___) | |___|  _ <| |___| |___| |\  | | |___|  _  |/ ___ \|  _ < 
//  |____/|_____| |_|   |____/ \____|_| \_\_____|_____|_| \_|  \____|_| |_/_/   \_\_| \_\
//
//  Function set char in screen by path
//  EBX - x
//  ECX - y
//  EDX - character
//
    void SYS_set_scrchar(int x, int y, char character) {
        __asm__ volatile(
            "movl $23, %%eax\n"
            "movl %0, %%ebx\n"
            "movl %1, %%ecx\n"
            "movl %2, %%edx\n"
            "int %3\n"
            :
            : "r"(x), "r"(y), "r"((int)character), "i"(SYSCALL_INTERRUPT)
            : "eax", "ebx", "ecx"
        );
    }
//
/////////////////////////////

void SYS_set_scrcolor(int x, int y, uint8_t color) {
    __asm__ volatile(
        "movl $24, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "movl %2, %%edx\n"
        "int %3\n"
        :
        : "r"(x), "r"(y), "r"((int)color), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx"
    );
}