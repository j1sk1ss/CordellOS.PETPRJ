#include "../../include/x86.h"

#define UNUSED_PORT             0x80

void i386_io_wait() {
    i386_outb(UNUSED_PORT, 0);
}