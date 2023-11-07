#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "file_system.h"
#include "ata.h"

#include "../shell/include/Keyboard.h"

#define STACK_SIZE              100
#define MEMORY_SIZE             100
#define VARIABLE_MEMORY_START   8
#define CONSTANT_SIZE     		0

#define INSTRUCTION_LENGTH      7
#define PARAMETERS_LENGTH       50
#define LINE_SIZE               50
#define VARIABLE_LENGTH         5
#define LABEL_LENGTH            5

struct intermediate_lang{
	int instruc_no;
	int opcode;
	int parameters[5];

	char* string;
	char* string_params[5];
};


struct symbol_table{
	char variable_name[VARIABLE_LENGTH];
	int address;
	int size;
};


struct blocks_table{
	char name[LABEL_LENGTH];
	int instr_no;  // instruction number after start
};

int asm_execute(char* file_data);

void display_symbol_table();
void display_intermediate_table();
void display_block_table();
void asm_executor(int *memory_array, int memory_index);