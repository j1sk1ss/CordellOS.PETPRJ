#include "../../include/e9.h"
#include <include/io.h>

void e9_print(char c) {
    i686_outb(0xE9, c);
}
