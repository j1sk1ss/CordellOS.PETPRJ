#ifndef DATETIME_H_
#define DATETIME_H_


#include "x86.h"


#define TICK_DELAY  1000000


extern unsigned char datetime_second;
extern unsigned char datetime_minute;
extern unsigned char datetime_hour;
extern unsigned char datetime_day;
extern unsigned char datetime_month;
extern unsigned int datetime_year;


void datetime_read_rtc();
void tick();
int get_ticks();


#endif