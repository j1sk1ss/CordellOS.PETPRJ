#include "../../include/x86.h"

#define UNUSED_PORT             0x80

void i686_io_wait() {
    x86_outb(UNUSED_PORT, 0);
}