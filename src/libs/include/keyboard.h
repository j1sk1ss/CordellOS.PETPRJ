#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_


#include <stdint.h>

#include "graphics.h"


#define KBD_DATA_PORT           0x60

#define HIDDEN_KEYBOARD         0
#define VISIBLE_KEYBOARD        1

#define DEL_BUTTON              '\252'
#define LSHIFT_BUTTON           '\252'
#define RSHIFT_BUTTON           '\253'
#define F4_BUTTON               '\254'
#define F3_BUTTON               '\255'
#define F2_BUTTON               '\7'
#define F1_BUTTON               '\6'

#define UP_ARROW_BUTTON         '\4'
#define DOWN_ARROW_BUTTON       '\3'
#define LEFT_ARROW_BUTTON       '\1'
#define RIGHT_ARROW_BUTTON      '\2'

#define ENTER_BUTTON            '\n'
#define BACKSPACE_BUTTON        '\b'

#define LSHIFT                  0x2A
#define RSHIFT                  0x36


void keyboard_read(int mode, uint8_t color, char* stop_list, char* buffer);

void input_read(int mode, uint8_t color, char* buffer);
void input_read_stop(int mode, uint8_t color, char* stop_list, char* buffer);
char get_char();
char wait_char();

#endif