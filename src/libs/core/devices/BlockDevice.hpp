#pragma once

#include "CharacterDevice.hpp"

enum class SeekPosition {
    StartPosition,
    CurrentPosition,
    EndPosition
};

class BlockDevice : public CharacterDevice {
    public:
        virtual ~BlockDevice();

        virtual size_t Read(uint8_t* data, size_t size) = 0;
        virtual size_t Write(const uint8_t* data, size_t size) = 0;
        virtual void Seek(SeekPosition type, int rel) = 0;

        virtual size_t Size() = 0;
};