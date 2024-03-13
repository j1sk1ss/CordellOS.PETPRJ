#include "../include/time.h"


void sleep_s(const uint16_t seconds)  {
    for (int i = 0; i < seconds * 10000; i++);
}

void sleep_ms(const uint32_t milliseconds) {
   for (int i = 0; i < milliseconds * 10000; i++);
}