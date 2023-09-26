#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    const char* Name;

    bool (*Probe)();                                                          // Exist device or no

    void (*Initialize)(uint8_t offsetPic1, uint8_t offsetPic2, bool autoEoi); // Enable device
    void (*Disable)();                                                        // Disable device

    void (*SendEndOfInterrupt)(int irq);                                      // Send end of interrupt to cpu

    void (*Mask)(int irq);                                                    // Mask IRQ by mask
    void (*Unmask)(int irq);                                                  // Unmask IRQ
} PICDriver;