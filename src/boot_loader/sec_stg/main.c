#include <stdint.h>;
//#include "stdio.h";
//#include "disk.h";
//#include "fat.h";

//void far* _data = (void far*)0x00500200;

void __attribute__((cdecl)) _start(uint16_t bootDrive) {
    clrscr();
    printf("Hi from sec_stg");
}