#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h" // complete
#include "string.h" // complete
#include "fat.h" // complete
#include "keyboard.h" // complete

#define STACK_SIZE              100
#define MEMORY_SIZE             100
#define VARIABLE_MEMORY_START   8
#define CONSTANT_SIZE     		0

#define INSTRUCTION_LENGTH      7
#define PARAMETERS_LENGTH       50
#define LINE_SIZE               50
#define VARIABLE_LENGTH         5
#define LABEL_LENGTH            5

/////////////////////////////////////////////////////////////////////////
//   ___ _   _ ____ _____ ____  _   _  ____ _____ ___ ___  _   _ ____  
//  |_ _| \ | / ___|_   _|  _ \| | | |/ ___|_   _|_ _/ _ \| \ | / ___| 
//   | ||  \| \___ \ | | | |_) | | | | |     | |  | | | | |  \| \___ \ 
//   | || |\  |___) || | |  _ <| |_| | |___  | |  | | |_| | |\  |___) |
//  |___|_| \_|____/ |_| |_| \_\\___/ \____| |_| |___\___/|_| \_|____/

	#define MOV_INSTRUCTION		1
	#define ADD_INSTRUCTION		3
	#define SUB_INSTRUCTION		4
	#define MUL_INSTRUCTION		5

	#define JMP_INSTRUCTION		6
	#define ELS_INSTRUCTION		6
	#define IF_INSTRUCTION		7
	#define EIF_INSTRUCTION		15

	#define EQ_INSTRUCTION		8
	#define LT_INSTRUCTION		9
	#define GT_INSTRUCTION		10
	#define LTEQ_INSTRUCTION	11
	#define GTEQ_INSTRUCTION	12
	#define NEQ_INSTRUCTION		28

	#define CLEAR_INSTRUCTION	27
	#define PRINTS_INSTRUCTION	17
	#define PRINT_INSTRUCTION	13
	#define PRINTL_INSTRUCTION	26
	#define READ_INSTRUCTION	14

	#define END_INSTRUCTION		16

	#define MKFILE_INSTRUCTION	18
	#define RMFILE_INSTRUCTION	19
	#define WFILE_INSTRUCTION	20
	#define RFILE_INSTRUCTION	21

	#define FOR_INSTRUCTION		22
	#define EFOR_INSTRUCTION	23
	#define WHILE_INSTRUCTION	24
	#define EWHILE_INSTRUCTION	25

//
////////////////////////////////////////

struct intermediate_lang{
	int instruction_position;
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

int asm_execute(char* file_data, struct User* user);

void display_symbol_table();
void display_intermediate_table();
void display_block_table();
void asm_executor(int *memory_array, int memory_index, int start, int end, struct User* user);