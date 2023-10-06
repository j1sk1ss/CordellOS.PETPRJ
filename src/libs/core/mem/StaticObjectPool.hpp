#pragma once
#include <stddef.h>

template <typename T, size_t PoolSize>
class StaticObjectPool {
    public:
        StaticObjectPool();
        T* Allocate();
        void Free(T* object);

    private:
        T _Objects[PoolSize] {};
        bool _ObjectInUse[PoolSize] {};
        size_t _Size;
};

template <typename T, size_t PoolSize>
StaticObjectPool<T, PoolSize>::StaticObjectPool() : _Size(0) {
    for (size_t i = 0; i < PoolSize; i++)
        _ObjectInUse[i] = false;
}

template <typename T, size_t PoolSize>
T* StaticObjectPool<T, PoolSize>::Allocate() {
    if (_Size >= PoolSize)
        return nullptr;

    for (size_t i = 0; i < PoolSize; i++) {
        size_t idx = (i + _Size) % PoolSize;
        if (!_ObjectInUse[idx]) {
            _ObjectInUse[idx] = true;
            _Size++;
            return &_Objects[idx];
        }
    }

    // should never happen
    return nullptr;
}

template <typename T, size_t PoolSize>
void StaticObjectPool<T, PoolSize>::Free(T* object) {
    if (object < _Objects || object >= _Objects + PoolSize)
        return;

    size_t idx = object - _Objects;
    _ObjectInUse[idx] = false;
    _Size--;
}