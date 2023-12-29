#include <stdint.h>
#include <include/hal.h>

#include "../user_land/include/user_land.h"


extern void _init();

void kernel_main(void) {
    _init();                            // global constructors
    mm_init(0x00200000);                // Kernel loads in 0x40000 and kernel size is 0x00010000. Malloc start in 0x50000 + 0x1000
    HAL_initialize();
    x86_init_keyboard();

    user_land_entry();
    //shell();

end:
    for (;;);
}