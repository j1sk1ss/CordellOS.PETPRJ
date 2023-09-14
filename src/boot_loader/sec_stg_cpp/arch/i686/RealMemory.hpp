#pragma once

#include<stdint.h>

template <typename T>
uint32_t ToSegOffset(T address) {
    uint32_t addressInteger = reinterpret_cast<uint32_t>(address);

    uint32_t segment    = (addressInteger >> 4) & 0xFFFF;
    uint32_t offset     = addressInteger & 0xF;

    return (segment << 16) | offset;
}

//
//     0x12345
//  ____    |_
//  segment | offset

// issue -> 0xFFFF

template <typename T>
T ToLinear(uint32_t addr) {
    uint32_t offset     = (uint32_t)(addr) & 0xFFFF;
    uint32_t segment    = (uint32_t)(addr) >> 16;

    return T(segment * 16 + offset);
}
