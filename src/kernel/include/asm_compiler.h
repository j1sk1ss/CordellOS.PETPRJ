#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "../shell/include/Keyboard.h"

#define stack_size              100
#define memory_size             100
#define variable_memory_start   8
#define const_variable_size     0

#define instruction_length      6
#define parametes_length        25
#define line_size               25
#define variable_length         5
#define label_length            5

struct intermediate_lang{
	int instruc_no;
	int opcode;
	int parameters[5];
};


struct symbol_table{
	char variable_name[variable_length];
	int address;
	int size;
};


struct blocks_table{
	char name[label_length];
	int instr_no;  // instruction number after start
};

int asm_execute(char* file_data);

void display_symbol_table();
void display_intermediate_table();
void display_block_table();
void asm_executor(int *memory_array, int memory_index);