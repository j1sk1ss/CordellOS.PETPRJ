#include "Debug.hpp"

#define MAX_OUTPUT_DEVICES 10

namespace {
    static const char* const g_LogSeverityColors[] = {
        [static_cast<int>(Debug::Level::Debug)]        = "\033[2;37m",
        [static_cast<int>(Debug::Level::Info)]         = "\033[37m",
        [static_cast<int>(Debug::Level::Warn)]         = "\033[1;33m",
        [static_cast<int>(Debug::Level::Error)]        = "\033[1;31m",
        [static_cast<int>(Debug::Level::Critical)]     = "\033[1;37;41m",
        [static_cast<int>(Debug::Level::Special)]      = "\033[2;32m",
    };
    
    static const char* const _colorReset = "\033[0m";

    struct {
        TextDevice* device;
        Debug::Level logLevel;
        bool colored;
    } _outputDevices[MAX_OUTPUT_DEVICES];
    
    int _outputDevicesCount;
}

namespace Debug {

    void AddOutputDevice(TextDevice* device, Level minLogLevel, bool colorOutput) {
        _outputDevices[_outputDevicesCount].device      = device;
        _outputDevices[_outputDevicesCount].logLevel    = minLogLevel;
        _outputDevices[_outputDevicesCount].colored     = colorOutput;

        _outputDevicesCount++;
    }

    static void Log(const char* module, Level logLevel, const char* fmt, va_list args) {
        for (int i = 0; i < _outputDevicesCount; i++) {
            if (logLevel < _outputDevices[i].logLevel)
                continue;

            // set color depending on level
            if (_outputDevices[i].colored)
                _outputDevices[i].device->Print(g_LogSeverityColors[static_cast<int>(logLevel)]);

            // write module
            _outputDevices[i].device->Printf("[%s] ", module);

            // print log
            _outputDevices[i].device->VPrintf(fmt, args);

            // reset color
            if (_outputDevices[i].colored)
                _outputDevices[i].device->Print(_colorReset);

            _outputDevices[i].device->Print('\n');
        }
    }

    void Debug(const char* module, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        Log(module, Level::Debug, fmt, args);
        va_end(args);
    }

    void Info(const char* module, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        Log(module, Level::Info, fmt, args);
        va_end(args);
    }

    void Warn(const char* module, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        Log(module, Level::Warn, fmt, args);
        va_end(args);
    }

    void Error(const char* module, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        Log(module, Level::Error, fmt, args);
        va_end(args);
    }

    void Critical(const char* module, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        Log(module, Level::Critical, fmt, args);
        va_end(args);
    }

    void Special(const char* module, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        Log(module, Level::Special, fmt, args);
        va_end(args);
    }
}