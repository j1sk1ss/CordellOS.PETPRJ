#include "SecStageAllocator.hpp"

SecStageAllocator::SecStageAllocator(void* base, uint32_t limit)
    : _Base(reinterpret_cast<uint8_t*>(base)), _Limit(limit) {}

void* SecStageAllocator::Allocate(size_t size) {
    if (_Allocated + size >= _Limit)
        return nullptr;

    void* ret = _Base + _Allocated;
    _Allocated += size;
    return ret;
}

void SecStageAllocator::Free(void* addr) {
    // NO-OP
}