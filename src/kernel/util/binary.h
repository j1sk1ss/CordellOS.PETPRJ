#pragma once

#define FLAG_SET(x, flag) x |= (flag)
#define FLAG_UNSET(x, flag) x &= ~(flag)

#define PUSH(stack, type, item) stack -= sizeof(type); *((type *) stack) = item
