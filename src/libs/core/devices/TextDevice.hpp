#pragma once

#include "CharacterDevice.hpp"

#include <stdarg.h>
#include <core/cpp/TypeTraits.hpp>

class TextDevice {
    public:
        TextDevice(CharacterDevice* dev);
        bool Print(char c);
        bool Print(const char* string);
        bool VPrintf(const char* string, va_list args);
        bool Printf(const char* string, ...);
        bool FormatBuffer(const char* msg, const void* buffer, size_t count);

        template<typename TNumber>
        bool Print(TNumber number, int radix);

    private:
        CharacterDevice* device;

        static const char _hexChars[];
};


template<typename TNumber>
bool TextDevice::Print(TNumber number, int radix) {
    typename MakeUnsigned<TNumber>::Type unsNumber;
    bool ok = true;

    if (IsSigned<TNumber>() && number < 0) {
        ok = ok && Print('-');
        unsNumber = -number;
    }
    else
        unsNumber = number;
    
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do  {
        typename MakeUnsigned<TNumber>::Type rem = number % radix;
        number /= radix;
        buffer[pos++] = _hexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        ok = ok && Print(buffer[pos]);

    return ok;
}

