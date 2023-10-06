#include "../include/debug.h"

#include <stdarg.h>
#include <include/vfs.h>

static const char* const _logSeverityColors[] = {
    [LVL_DEBUG]     = "\033[2;37m",
    [LVL_INFO]      = "\033[2;34m",
    [LVL_WARN]      = "\033[2;33m",
    [LVL_ERROR]     = "\033[2;31m",
    [LVL_CRITICAL]  = "\033[2;31m",
    [CORDELL_MSG]   = "\033[2;32m"
};

static const char* const _colorReset = "\033[0m";

void logf(const char* module, DebugLevel level, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    // set color with log lovel
    fputs(_logSeverityColors[level], VFS_FD_DEBUG);

    // write module
    fprintf(VFS_FD_DEBUG, "[CORDELL OS]: [%s] ", module);

    // Write text
    vfprintf(VFS_FD_DEBUG, fmt, args, 0);

    // reset format
    fputs(_colorReset, VFS_FD_DEBUG);
    fputc('\n', VFS_FD_DEBUG, 0);

    va_end(args);
}