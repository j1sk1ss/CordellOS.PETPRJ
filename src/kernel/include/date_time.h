#ifndef __DATETIME_H__
#define __DATETIME_H__

#include "x86.h"

extern unsigned char datetime_second;
extern unsigned char datetime_minute;
extern unsigned char datetime_hour;
extern unsigned char datetime_day;
extern unsigned char datetime_month;
extern unsigned int datetime_year;

void datetime_read_rtc();

#endif