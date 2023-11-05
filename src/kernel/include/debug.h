#include "stdio.h"

// typedef enum {
//     LVL_DEBUG       = 0,
//     LVL_INFO        = 1,
//     LVL_WARN        = 2,
//     LVL_ERROR       = 3,
//     LVL_CRITICAL    = 4,
//     CORDELL_MSG     = 5
// } DebugLevel;

void logf(const char* module, int level, const char* fmt, ...);

#define log_debug(module, ...) logf(module, 0, __VA_ARGS__);
#define log_info(module, ...) logf(module, 1, __VA_ARGS__);
#define log_warn(module, ...) logf(module, 2, __VA_ARGS__);
#define log_error(module, ...) logf(module, 3, __VA_ARGS__);
#define log_critical(module, ...) logf(module, 4, __VA_ARGS__);
#define log_cordell(module, ...) logf(module, 5, __VA_ARGS__);