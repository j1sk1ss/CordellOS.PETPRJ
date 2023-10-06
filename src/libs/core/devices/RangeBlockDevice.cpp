#include "RangeBlockDevice.hpp"
#include <core/cpp/Algorithm.hpp>

RangeBlockDevice::RangeBlockDevice()
    : _Device(nullptr),
      _RangeBegin(0),
      _RangeSize(0)
{
}

void RangeBlockDevice::Initialize(BlockDevice* device, size_t rangeBegin, size_t rangeSize) {
    _Device        = device;
    _RangeBegin    = rangeBegin;
    _RangeSize     = rangeSize;

    _Device->Seek(SeekPosition::StartPosition, rangeBegin);
}

size_t RangeBlockDevice::Read(uint8_t* data, size_t size) {
    if (_Device == nullptr)
        return 0;

    size = Min(size, Size() - Position());
    return _Device->Read(data, size);
}

size_t RangeBlockDevice::Print(const uint8_t* data, size_t size) {
    if (_Device == nullptr)
        return 0;

    size = Min(size, Size() - Position());
    return _Device->Print(data, size);
}

bool RangeBlockDevice::Seek(SeekPosition pos, int rel) {
    if (_Device == nullptr)
        return false;
        
    switch (pos) {
        case SeekPosition::StartPosition:
            return _Device->Seek(SeekPosition::StartPosition, _RangeBegin + rel);
        
        case SeekPosition::CurrentPosition:
            return _Device->Seek(SeekPosition::CurrentPosition, rel);

        case SeekPosition::EndPosition:
            return _Device->Seek(SeekPosition::StartPosition, _RangeBegin + _RangeSize);

        default:
            return false;
    }
}

size_t RangeBlockDevice::Size() {
    return _RangeSize;
}

size_t RangeBlockDevice::Position() {
    return _Device->Position() - _RangeBegin;
}