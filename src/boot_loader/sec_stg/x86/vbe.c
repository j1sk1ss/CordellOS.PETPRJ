#include "../include/vbe.h"
#include "../include/x86.h"
#include "../include/stdio.h"
#include "../include/memory.h"

#include <stdbool.h>

bool vbe_getControllerInfo(VbeInfoBlock* info) {
    int ret = x86_video_getVbeInfo(info);

    printf("Controller memory: %x\n", info->TotalMemory);

    if (ret == 0) {
        info->VideoModePtr = SEGOFF2LINE(info->VideoModePtr);
        return true;
    }

    printf("VBE is not avaliable!\n");
    return false;
}

bool vbe_getModeInfo(uint16_t mode, VBEModeInfoStructure* info) {
    int ret = x86_video_getVbeModeInfo(mode, info);
    if (ret == 0) {
        return true;
    }

    return false;
}

bool vbe_setMode(uint16_t mode) {
    return x86_video_setVbeMode(mode) == 0;
}