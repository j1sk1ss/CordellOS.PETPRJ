#include "RangeBlockDevice.hpp"

#include <core/cpp/Algorithm.hpp>

RangeBlockDevice::RangeBlockDevice() : m_Device(nullptr), m_RangeBegin(0), m_RangeSize(0) {  } 

void RangeBlockDevice::Initialize(BlockDevice* device, size_t rangeBegin, size_t rangeSize) {
    m_Device        = device;
    m_RangeBegin    = rangeBegin;
    m_RangeSize     = rangeSize;

    m_Device->Seek(SeekPosition::StartPosition, rangeBegin);
}

size_t RangeBlockDevice::Read(uint8_t* data, size_t size) {
    size = Min(size, Size() - Position());
    return m_Device->Read(data, size);
}

size_t RangeBlockDevice::Print(const uint8_t* data, size_t size) {
    if (m_Device == nullptr)
        return 0;

    size = Min(size, Size() - Position());
    return m_Device->Print(data, size);
}

bool RangeBlockDevice::Seek(SeekPosition pos, int rel) {
    if (m_Device == nullptr)
        return false;

    switch (pos) {
        case SeekPosition::StartPosition:
            m_Device->Seek(SeekPosition::StartPosition, m_RangeBegin + rel);
            break;
        
        case SeekPosition::CurrentPosition:
            m_Device->Seek(SeekPosition::CurrentPosition, rel);
            break;

        case SeekPosition::EndPosition:
            m_Device->Seek(SeekPosition::CurrentPosition, m_RangeBegin + m_RangeSize);

        default:
            break;
    }
}

size_t RangeBlockDevice::Size() {
    return m_RangeSize;
}

size_t RangeBlockDevice::Position() {
    return m_Device->Position() - m_RangeBegin;
}