#pragma once

#include "CharacterDevice.hpp"

enum class SeekPosition {
    StartPosition,
    CurrentPosition,
    EndPosition
};

class BlockDevice : public CharacterDevice {
    public:
        virtual bool Seek(SeekPosition pos, int rel) = 0;
        virtual size_t Size() = 0;
        virtual size_t Position() = 0;
};