#pragma once

#include <stdint.h>
#include <stddef.h>

class CharacterDevice {
    public:
        virtual ~CharacterDevice() {  }

        virtual size_t Read(uint8_t* data, size_t size) = 0;
        virtual size_t Print(const uint8_t* data, size_t size) = 0;
};