#include "SecStageAllocator.hpp"

SecStageAllocator::SecStageAllocator(void* base, uint32_t limit)
    : m_Base(reinterpret_cast<uint8_t*>(base)), m_Limit(limit) {}

void* SecStageAllocator::Allocate(size_t size) {
    if (m_Allocated + size >= m_Limit)
        return nullptr;

    void* ret = m_Base + m_Allocated;
    m_Allocated += size;
    return ret;
}

void SecStageAllocator::Free(void* addr) {
    // NO-OP
}