#pragma once

#include "BlockDevice.hpp"

class RangeBlockDevice : public BlockDevice {
    public:
        RangeBlockDevice();

        void Initialize(BlockDevice* device, size_t rangeBegin, size_t rangeSize);
        virtual size_t Read(uint8_t* data, size_t size) override;
        virtual size_t Print(const uint8_t* data, size_t size) override;
        virtual bool Seek(SeekPosition pos, int rel) override;
        virtual size_t Size() override;
        virtual size_t Position() override;

    private:
        BlockDevice* _Device;
        size_t _RangeBegin;
        size_t _RangeSize;
};