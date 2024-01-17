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
//  EBX - color
//  ECX - character
//
    void SYS_cputc(uint8_t color, char character) {
        __asm__ volatile(
            "movl $13, %%eax\n"
            "movl %0, %%ebx\n"
            "movl %1, %%ecx\n"
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
            : "eax", "ebx", "ecx", "edx"
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
            : "eax", "ebx", "ecx", "edx"
        );

        return directory;
    }
//
/////////////////////////////

int SYS_cexists(const char* path) {
    int result = 0;
    uint32_t path_ptr = (uint32_t)path;

    __asm__ volatile(
        "movl $15, %%eax\n"
        "movl %1, %%ebx\n"
        "int %2\n"
        "movl %%eax, %0\n"
        : "=r"(result)
        : "r"(path_ptr), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx"
    );

    return result;
}

void SYS_mkfile(const char* path, const char* name, const char* extension) {
    uint32_t file_path_ptr = (uint32_t)path;
    uint32_t file_name_ptr = (uint32_t)name;
    uint32_t file_extension_ptr = (uint32_t)extension;

    __asm__ volatile(
        "movl $16, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "movl %2, %%edx\n"
        "int %3\n"
        :
        : "r"(file_path_ptr), "r"(file_name_ptr), "r"(file_extension_ptr), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );
}

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
        : "eax", "ebx", "ecx", "edx"
    );
}

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
        : "eax", "ebx", "ecx", "edx"
    );
}

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
        : "r"(file_path_ptr), "r"(args), "r"(argv), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );

    return result;
}
