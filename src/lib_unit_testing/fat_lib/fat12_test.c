#include "fat12_test.h"
#include "../../src/unit_testing/testing.h"
#include "../../src/libs/fat_lib/fat.h"

#include <assert.h>

void fat_lib_fat12_smoke_test() {
    DISK disk;
    FAT_initialize(&disk);

    assert(disk.);
}