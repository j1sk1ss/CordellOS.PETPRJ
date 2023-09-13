#include "VGATextDevice.hpp"
#include "IO.hpp"

namespace arch {
    namespace i686 {

        static inline constexpr unsigned ScreenWidth    = 80;
        static inline constexpr unsigned ScreenHeight   = 25;

        static inline constexpr uint8_t DefaultColor = 0x7;

        uint8_t* VGATextDevice::_screenBuffer = (uint8_t*)0xB8000;

        VGATextDevice::VGATextDevice() : _screenX(0), _screenY(0) {
            Clear();
        }

        size_t VGATextDevice::Read(uint8_t* data, size_t size) {
            return 0;
        }

        size_t VGATextDevice::Print(const uint8_t* data, size_t size) {
            for (size_t i = 0; i < size; i++)
                PutChar(data[i]);

            return size;
        }

        void VGATextDevice::PutChar(int x, int y, char c) {
            _screenBuffer[2 * (y * ScreenWidth + x)] = c;
        }

        void VGATextDevice::PutColor(int x, int y, uint8_t color) {
            _screenBuffer[2 * (y * ScreenWidth + x) + 1] = color;
        }

        char VGATextDevice::GetChar(int x, int y) {
            return _screenBuffer[2 * (y * ScreenWidth + x)];
        }

        uint8_t VGATextDevice::GetColor(int x, int y) {
            return _screenBuffer[2 * (y * ScreenWidth + x) + 1];
        }

        void VGATextDevice::SetCursor(int x, int y) {
            int pos = y * ScreenWidth + x;

            Out(0x3D4, 0x0F);
            Out(0x3D5, (uint8_t)(pos & 0xFF));
            Out(0x3D4, 0x0E);
            Out(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
        }

        void VGATextDevice::Clear() {
            for (int y = 0; y < ScreenHeight; y++)
                for (int x = 0; x < ScreenWidth; x++) {
                    PutChar(x, y, '\0');
                    PutColor(x, y, DefaultColor);
                }

            _screenX = 0;
            _screenY = 0;
            SetCursor(_screenX, _screenY);
        }

        void VGATextDevice::Scrollback(int lines) {
            for (int y = lines; y < ScreenHeight; y++)
                for (int x = 0; x < ScreenWidth; x++) {
                    PutChar(x, y - lines, GetChar(x, y));
                    PutColor(x, y - lines, GetColor(x, y));
                }

            for (int y = ScreenHeight - lines; y < ScreenHeight; y++)
                for (int x = 0; x < ScreenWidth; x++) {
                    PutChar(x, y, '\0');
                    PutColor(x, y, DefaultColor);
                }

            _screenY -= lines;
        }

        void VGATextDevice::PutChar(char c) {
            switch (c) {
                case '\n':
                    _screenX = 0;
                    _screenY++;
                    break;
            
                case '\t':
                    for (int i = 0; i < 4 - (_screenX % 4); i++)
                        PutChar(' ');
                    break;

                case '\r':
                    _screenX = 0;
                    break;

                default:
                    PutChar(_screenX, _screenY, c);
                    _screenX++;
                    break;
            }

            if (_screenX >= ScreenWidth) {
                _screenY++;
                _screenX = 0;
            }
            
            if (_screenY >= ScreenHeight)
                Scrollback(1);

            SetCursor(_screenX, _screenY);
        }

    }
}