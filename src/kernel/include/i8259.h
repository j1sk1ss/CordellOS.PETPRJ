#ifndef I8259_H_
#define I8259_H_


#include <stdint.h>
#include <stdbool.h>

#include "pic.h"
#include "x86.h"


const PICDriver* i8259_getDriver();
uint16_t i8259_readIRQRequestRegisters();
uint16_t i8259_readIRQInServiceRegisters();

#endif