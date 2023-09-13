#include <core/devices/CharacterDevice.hpp>

namespace arch {
    namespace i686 {

        class E9Device : public CharacterDevice {
            public:
                virtual size_t Read(uint8_t* data, size_t size);
                virtual size_t Print(const uint8_t* data, size_t size);
        };
        
    }
}