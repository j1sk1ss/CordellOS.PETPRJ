#include "../include/netutils.h"


uint16_t flipEndian16(uint16_t short_int) {
    uint32_t first_byte  = *((uint8_t*)(&short_int));
    uint32_t second_byte = *((uint8_t*)(&short_int) + 1);
    return (first_byte << 8) | (second_byte);
}

uint32_t flipEndian32(uint32_t long_int) {
    uint32_t first_byte  = *((uint8_t*)(&long_int));
    uint32_t second_byte = *((uint8_t*)(&long_int) + 1);
    uint32_t third_byte  = *((uint8_t*)(&long_int)  + 2);
    uint32_t fourth_byte = *((uint8_t*)(&long_int) + 3);
    return (first_byte << 24) | (second_byte << 16) | (third_byte << 8) | (fourth_byte);
}

/*
 * Flip two parts within a byte
 * For example, 0b11110000 will be 0b00001111 instead
 * This is necessary because endiness is also relevant to byte, where there are two fields in one byte.
 * number_bits: number of bits of the less significant field
 * */
uint8_t swapByteParts(uint8_t byte, int num_bits) {
    uint8_t t = byte << (8 - num_bits);
    return t | (byte >> num_bits);
}

uint8_t hostToNetByte(uint8_t byte, int num_bits) {
    return swapByteParts(byte, num_bits);
}

uint8_t netToHostByte(uint8_t byte, int num_bits) {
    return swapByteParts(byte, 8 - num_bits);
}

uint16_t hostToNet16(uint16_t hostshort) {
    return flipEndian16(hostshort);
}

uint32_t hostToNet32(uint32_t hostlong) {
    return flipEndian32(hostlong);
}

uint16_t netToHost16(uint16_t netshort) {
    return flipEndian16(netshort);
}

uint32_t netToHost32(uint32_t netlong) {
    return flipEndian32(netlong);
}