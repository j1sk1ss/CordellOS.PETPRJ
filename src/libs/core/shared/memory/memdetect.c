#include "../include/x86.h"
#include "../include/memdetect.h"
#include "../include/stdio.h"

#include <boot/bootparams.h>

#define MAX_REGIONS 256

MemoryRegion _memRegions[MAX_REGIONS];
int _memRegionsCount = 0;

void memory_detect(MemoryInfo* memoryInfo) {
    E820MemoryBlock block;
    uint32_t continuation = 0;
    int ret;

    _memRegionsCount = 0;
    ret = x86_e820GetNextBlock(&block, &continuation);

    while (ret > 0 && continuation != 0) {
        _memRegions[_memRegionsCount].Begin     = block.Base;
        _memRegions[_memRegionsCount].Length    = block.Length;
        _memRegions[_memRegionsCount].Type      = block.Type;
        _memRegions[_memRegionsCount].ACPI      = block.ACPI;

        ++_memRegionsCount;

        printf("E820: base=0x%llx length=0x%llx type=0x%x\n", block.Base, block.Length, block.Type);

        ret = x86_e820GetNextBlock(&block, &continuation);
    }

    // fill mem inf
    memoryInfo->BlockCount  = _memRegionsCount;
    memoryInfo->Regions     = _memRegions;
}
