#include "../../include/time.h"

void TIME_sleep_seconds(const uint16_t seconds)  {
    __asm__ __volatile__("int $0x80" : : "a"(2), "b"(seconds * 1000) );
}

// Sleep for a given number of milliseconds
void TIME_sleep_milliseconds(const uint32_t milliseconds) {
    __asm__ __volatile__("int $0x80" : : "a"(2), "b"(milliseconds) );
}