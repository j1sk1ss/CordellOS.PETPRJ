#include <stdint.h>


uint16_t flipEndian16(uint16_t short_int);
uint32_t flipEndian32(uint32_t long_int);

uint8_t swapByteParts(uint8_t byte, int num_bits);
uint8_t hostToNetByte(uint8_t byte, int num_bits);
uint8_t netToHostByte(uint8_t byte, int num_bits);

uint16_t hostToNet16(uint16_t hostshort);
uint32_t hostToNet32(uint32_t hostlong);
uint16_t netToHost16(uint16_t netshort);
uint32_t netToHost32(uint32_t netlong);