#include "../include/debug.h"

#include <stdarg.h>
#include <include/vfs.h>

static const char* const _logSeverityColors[] = {
    [0]     = "\033[2;37m",
    [1]      = "\033[2;34m",
    [2]      = "\033[2;33m",
    [3]     = "\033[2;31m",
    [4]  = "\033[2;31m",
    [5]   = "\033[2;32m"
};

static const char* const _colorReset = "\033[0m";

void logf(const char* module,int level, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    // set color with log lovel
    fputs(_logSeverityColors[level], VFS_FD_DEBUG, 0);

    // write module
    fprintf(VFS_FD_DEBUG, "[CORDELL OS]: [%s] ", module);

    // Write text
    vfprintf(VFS_FD_DEBUG, fmt, args, 0);

    // reset format
    fputs(_colorReset, VFS_FD_DEBUG, 0);
    fputc('\n', VFS_FD_DEBUG, 0);

    va_end(args);
}