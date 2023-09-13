#include "TextDevice.hpp"

enum class FormatState  {
    Normal      = 0,
    Length      = 1,
    LengthShort = 2,
    LengthLong  = 3,
    Spec        = 4,
};

enum class FormatLength  {
    Default       = 0,
    ShortShort    = 1,
    Short         = 2,
    Long          = 3,
    LongLong      = 4,
};

const char TextDevice::_hexChars[] = "0123456789abcdef";

TextDevice::TextDevice(CharacterDevice* dev) : device(dev) {}


bool TextDevice::Print(char c) {
    return device->Print(reinterpret_cast<const uint8_t*>(&c), sizeof(c)) == sizeof(c);
}

bool TextDevice::Print(const char* string) {
    bool ok = true;
    while(*string && ok) {
        ok = ok && Print(*string);
        string++;
    }

    return ok;
}

bool TextDevice::VPrintf(const char* string, va_list args) {
    FormatState state   = FormatState::Normal;
    FormatLength length = FormatLength::Default;

    int radix   = 10;
    bool sign   = false;
    bool number = false;
    bool ok     = true;

    while (*string) {
        switch (state) {
            case FormatState::Normal:
                switch (*string) {
                    case '%':   state = FormatState::Length;
                                break;
                    default:    ok = ok && Print(*string);
                                break;
                }

                break;

            case FormatState::Length:
                switch (*string) {
                    case 'h':   length = FormatLength::Short;
                                state = FormatState::LengthShort;
                                break;
                    case 'l':   length = FormatLength::Long;
                                state = FormatState::LengthLong;
                                break;
                    default:    goto FormatState_Spec;
                }

                break;

            case FormatState::LengthShort:
                if (*string == 'h') {
                    length  = FormatLength::ShortShort;
                    state   = FormatState::Spec;
                }
                else goto FormatState_Spec;

                break;

            case FormatState::LengthLong:
                if (*string == 'l') {
                    length  = FormatLength::LongLong;
                    state   = FormatState::Spec;
                }
                else goto FormatState_Spec;

                break;

            case FormatState::Spec:
            FormatState_Spec:
                switch (*string) {
                    case 'c':   ok = ok && Print((char)va_arg(args, int));
                                break;

                    case 's':   
                                ok = ok && Print(va_arg(args, const char*));
                                break;

                    case '%':   ok = ok && Print('%');
                                break;

                    case 'd':
                    case 'i':   
                        radix   = 10; 
                        sign    = true; 
                        number  = true;
                    break;

                    case 'u':   
                        radix   = 10; 
                        sign    = false; 
                        number  = true;
                    break;

                    case 'X':
                    case 'x':
                    case 'p':   
                        radix   = 16; 
                        sign    = false; 
                        number  = true;
                    break;

                    case 'o':   
                        radix   = 8; 
                        sign    = false; 
                        number  = true;
                    break;

                    // ignore invalid spec
                    default:    break;
                }

                if (number) {
                    if (sign) {
                        switch (length) {
                            case FormatLength::ShortShort:
                            case FormatLength::Short:
                            case FormatLength::Default:     ok = ok && Print(va_arg(args, int), radix);
                                                            break;

                            case FormatLength::Long:        ok = ok && Print(va_arg(args, long), radix);
                                                            break;

                            case FormatLength::LongLong:    ok = ok && Print(va_arg(args, long long), radix);
                                                            break;
                        }
                    }
                    else {
                        switch (length) {
                            case FormatLength::ShortShort:
                            case FormatLength::Short:
                            case FormatLength::Default:     ok = ok && Print(va_arg(args, unsigned int), radix);
                                                            break;
                                                            
                            case FormatLength::Long:        ok = ok && Print(va_arg(args, unsigned  long), radix);
                                                            break;

                            case FormatLength::LongLong:    ok = ok && Print(va_arg(args, unsigned  long long), radix);
                                                            break;
                        }
                    }
                }

                // reset state
                state   = FormatState::Normal;
                length  = FormatLength::Default;
                radix   = 10;
                sign    = false;
                number  = false;

                break;
        }

        string++;
    }

    return ok;
}

bool TextDevice::Printf(const char* string, ...) {
    va_list args;
    va_start(args, string);
    bool ok = VPrintf(string, args);
    va_end(args);

    return ok;
}

bool TextDevice::FormatBuffer(const char* msg, const void* buffer, size_t count) {
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    bool ok = true;
    
    ok = ok && Print(msg);
    for (uint16_t i = 0; i < count; i++) {
        ok = ok && Print(_hexChars[u8Buffer[i] >> 4]);
        ok = ok && Print(_hexChars[u8Buffer[i] & 0xF]);
    }

    ok = ok && Print("\n");

    return ok;
}