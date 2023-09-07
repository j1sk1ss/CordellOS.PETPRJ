#pragma once

#include <stdint.h>

void i686_pic_configure(uint8_t offsetPic1, uint8_t offsetPic2);
void i686_sendEndOfInterrupt(int irq);
void i686_pic_disable();
void i686_pic_mask(int irq);
void i686_pic_unmask(int irq);
uint16_t i686_pic_readIRQRequestRegisters();
uint16_t i686_pic_readIRQInServiceRegisters();