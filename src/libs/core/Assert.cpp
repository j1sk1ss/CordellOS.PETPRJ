#include "Assert.hpp"

#include <arch/i686/IO.hpp>
#include <Debug.hpp>

bool Assert_(const char* condition, const char* filename, int line, const char* function) {
    Debug::Critical("Assert", "Failed! %s", condition);

    arch::i686::Panic();
    return false;
}