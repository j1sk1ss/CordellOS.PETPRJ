#include "e9.h"
#include <arch/i686/io.h>

void e9_print(char c)
{
    i686_outb(0xE9, c);
}
