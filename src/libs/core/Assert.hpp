#pragma once

#define Assert(condition)  ((condition) || Assert_(#condition, __FILE__, __LINE__, __FUNCTION__))

bool Assert_(const char* condition, const char* filename, int line, const char* function);
