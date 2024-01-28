#include "../include/stdlib.h"


//====================================================================
// Function start task at address with entered name
// EBX - name
// ECX - address 
void tstart(char* name, uint32_t address) {
    __asm__ volatile(
        "movl $26, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "int %2\n"
        :
        : "r"(name), "r"(address), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx"
    );
}

//====================================================================
// Function kill current task
void tkill() {
    __asm__ volatile(
        "movl $27, %%eax\n"
        "movl $1, %%ebx\n"
        "movl $1, %%edx\n"
        "int %0\n"
        :
        : "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx", "edx"
    );
}

//====================================================================
//  Sleep function
//  EDX - time
void sleep(int milliseconds) {
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

//====================================================================
//  Return date time from cmos in short*
//  ECX - pointer to array
//
//  data[0] - seconds
//  data[1] - minute
//  data[2] - hour
//  data[3] - day
//  data[4] - month
//  data[5] - year
void get_datetime(short* data) {
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

//====================================================================
//  Allocate memory and return pointer
//  EBX - size
//  EAX - returned pointer to allocated memory
void* malloc(uint32_t size) {
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

void* realloc(void* ptr, size_t size) {
    void* new_data = NULL;
    if (size) {
        if(!ptr) return malloc(size);

        new_data = malloc(size);
        if(new_data) {
            memcpy(new_data, ptr, size);
            free(ptr);
        }
    }

    return new_data;
}

void* calloc(size_t nelem, size_t elsize) {
    void* tgt = malloc(nelem * elsize);
    if (tgt != NULL) 
        memset(tgt, 0, nelem * elsize);

    return tgt;
}

//====================================================================
//  Free alocated memory
//  EBX - pointer to allocated memory
void free(void* ptr) {
    __asm__ volatile(
        "movl $8, %%eax\n"
        "movl %0, %%ebx\n"
        "int $0x80\n"
        :
        : "r" (ptr)
        : "%eax", "%ebx"
    );
}

//====================================================================
//  Read file content by path
//  ECX - path
//  EAX - returned data
char* fread(const char* path) {
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

//====================================================================
//  Write file to content (if it exists) by path (rewrite all content)
//  EBX - path
//  ECX - data
void fwrite(const char* path, const char* data) {
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

//====================================================================
//  Returns linked list of dir content by path
//  EBX - path
//  ECX - pointer to directory
struct UFATDirectory* opendir(const char* path) {
    uint32_t path_ptr = (uint32_t)path;
    struct UFATDirectory* directory = (struct UFATDirectory*)malloc(sizeof(struct UFATDirectory));
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

//====================================================================
//  Function for checking if content exist by this path
//  EBX - path
//  ECX - result (0 - nexists)
int cexists(const char* path) {
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

//====================================================================
//  This function creates file
//  EBX - path
//  RCX - name (with extention)
void mkfile(const char* path, const char* name) {
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

//====================================================================
//  This function creates directory
//  EBX - path
//  ECX - name
void mkdir(const char* path, const char* name) {
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

//====================================================================
//  This function remove content
//  EBX - path
//  ECX - name (if file - with extention)
void rmcontent(const char* path, const char* name) {
    uint32_t path_ptr = (uint32_t)path;
    uint32_t content_ptr = (uint32_t)name;

    __asm__ volatile(
        "movl $18, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "int %2\n"
        :
        : "r"(path_ptr), "r"(content_ptr), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx"
    );
}

//====================================================================
// This function change content meta by path
// EBX - path
// ECX - new meta
void chgcontent(const char* path, struct udirectory_entry* meta) {
    uint32_t path_ptr = (uint32_t)path;
    uint32_t content_ptr = (uint32_t)meta;

    __asm__ volatile(
        "movl $25, %%eax\n"
        "movl %0, %%ebx\n"
        "movl %1, %%ecx\n"
        "int %2\n"
        :
        : "r"(path_ptr), "r"(content_ptr), "i"(SYSCALL_INTERRUPT)
        : "eax", "ebx", "ecx"
    );
}

//====================================================================
//  Function that executes ELF file
//  EAX - result CODE
//  EBX - path
//  ECX - args (count)
//  EDX - argv (array of args)
int fexec(char* path, int args, char** argv) {
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