#pragma once

#include <stdint.h>

#define MAX_SYSCALLS 10 

typedef enum {
    SYSCALL_TEST0  = 0,
    SYSCALL_TEST1  = 1,
    SYSCALL_SLEEP  = 2,
    SYSCALL_MALLOC = 3,
    SYSCALL_FREE   = 4,
    SYSCALL_WRITE  = 5,
    SYSCALL_OPEN   = 6,
    SYSCALL_CLOSE  = 7,
    SYSCALL_READ   = 8,
    SYSCALL_SEEK   = 9,
} system_call_numbers;

typedef enum {
    O_RDONLY = 0x0,     // Read only      - 1 of 3 required
    O_WRONLY = 0x1,     // Write only     - 1 of 3 required
    O_RDWR   = 0x2,     // Read and write - 1 of 3 required
    O_CREAT  = 0x4,     // Create if not exist
    O_APPEND = 0x8,     // Always write at end of file position
    O_TRUNC  = 0x10,    // Truncate file size and pos to 0 on open
} open_flag_t;

typedef enum {
    SEEK_SET, 
    SEEK_CUR,
    SEEK_END,
} whence_value_t;

int32_t open(const char *filepath, open_flag_t flags);
int32_t close(const int32_t fd);
int32_t read(const int32_t fd, const void *buf, const uint32_t len);
int32_t write(const int32_t fd, const void *buf, const uint32_t len);
int32_t seek(const int32_t fd, const int32_t offset, const whence_value_t whence);