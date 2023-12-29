#include "../../include/asm_compiler.h"


extern int symbol_index;
extern int intermediate_index;
extern int blocks_index;
extern struct intermediate_lang **intermediate_table;
extern struct symbol_table **symbol_tab;
extern struct blocks_table **block_tab;

void display_symbol_table() {
	printf("\n---------------Symbol Table is ----------\n");
	for (int i = 0; i < symbol_index; i++)
		printf("\nvariable name is %s,address is %d, size is %d", symbol_tab[i]->variable_name, symbol_tab[i]->address, symbol_tab[i]->size);

	return;
}

void display_intermediate_table() {
	printf("\n---------------Instruction Table is ----------\n");
	for (int i = 0; i < intermediate_index; i++){
		printf("\n%d : %d : ", intermediate_table[i]->instruction_position, intermediate_table[i]->opcode);
		for (int j = 0; intermediate_table[i]->parameters[j] != -1; j++)
			printf(" %d", intermediate_table[i]->parameters[j]);
	}

	printf("\n");
	return;
}

void display_block_table() {
	printf("\n---------------Block Table is ----------\n");
	for (int i = 0; i < blocks_index; i++)
		printf("\n Label is : %s and address is : %d ", block_tab[i]->name, block_tab[i]->instr_no);

	printf("\n");
	return;
}

int check_condition(int operand1, int operand2, int opcode){
	switch(opcode){
		case EQ_INSTRUCTION: 
			if (operand1 == operand2)
				return 1;
		break;

		case LT_INSTRUCTION:
			if (operand1 < operand2)
				return 1;
		break;

		case GT_INSTRUCTION: 
			if (operand1 > operand2)
				return 1;
		break;

		case LTEQ_INSTRUCTION: 
			if (operand1 <= operand2)
				return 1;
		break;

		case GTEQ_INSTRUCTION: 
			if (operand1 >= operand2)
				return 1;
		break;

		case NEQ_INSTRUCTION: 
			if (operand1 != operand2)
				return 1;
		break;
	}
	
	return 0;
}

void asm_executor(int *memory_array, int memory_index, int start, int end, struct User* user) {
	for (int i = start; i < end;) {  // iterating on the intermediate language table
		switch (intermediate_table[i]->opcode) {
			case READ_INSTRUCTION:  
				printf("\n\r");
                memory_array[intermediate_table[i]->parameters[0]] = atoi(keyboard_read(VISIBLE_KEYBOARD, -1));
			break; // READ Instruction //

			case MOV_INSTRUCTION: 
                memory_array[intermediate_table[i]->parameters[0]] = memory_array[intermediate_table[i]->parameters[1]]; 
		    break; // MOVE Instruction //

			case ADD_INSTRUCTION:
                memory_array[intermediate_table[i]->parameters[0]] = memory_array[intermediate_table[i]->parameters[1]] +
																		 memory_array[intermediate_table[i]->parameters[2]];
			break; // ADD Instruction //

			case SUB_INSTRUCTION:
                memory_array[intermediate_table[i]->parameters[0]] = memory_array[intermediate_table[i]->parameters[1]] -
																		 memory_array[intermediate_table[i]->parameters[2]];
		    break; // SUB Instruction //

			case MUL_INSTRUCTION:
                memory_array[intermediate_table[i]->parameters[0]] = memory_array[intermediate_table[i]->parameters[1]] *
																		 memory_array[intermediate_table[i]->parameters[2]];
		    break; // MUL Instruction //

			case PRINT_INSTRUCTION: 
                printf("\n%d\n", memory_array[intermediate_table[i]->parameters[0]]);
	    	break;  // PRINT Instruction //

			case PRINTS_INSTRUCTION: 
                printf("%s", intermediate_table[i]->string);
	    	break;  // PRINTS Instruction //

			case PRINTL_INSTRUCTION:
                printf("\n%s", intermediate_table[i]->string);
	    	break;  // PRINTL Instruction //

			case MKFILE_INSTRUCTION:
				create_file(user->read_access, user->write_access, user->edit_access, 
				intermediate_table[i]->string_params[0], "txt", ATA_find_empty_sector(FILES_SECTOR_OFFSET));
			break;

			case RMFILE_INSTRUCTION:
				if (file_exist(intermediate_table[i]->string) == 1)
					if (user->edit_access <= find_file(intermediate_table[i]->string)->edit_level)
						delete_file(intermediate_table[i]->string);
			break;

			case WFILE_INSTRUCTION:
				struct File* wfile = find_file(intermediate_table[i]->string_params[0]);
				if (wfile != NULL)
					if (user->write_access <= wfile->write_level)
						write_file(wfile, intermediate_table[i]->string_params[1]);
			break;

			case RFILE_INSTRUCTION:
				struct File* rfile = find_file(intermediate_table[i]->string);
				if (rfile != NULL)
					if (user->read_access <= rfile->read_level) {
						int data = atoi(read_file(rfile));
						memory_array[intermediate_table[i]->parameters[0]] = data; 
					}
			break;

			case IF_INSTRUCTION: 
                if (check_condition(memory_array[intermediate_table[i]->parameters[0]], memory_array[intermediate_table[i]->parameters[1]],
                    intermediate_table[i]->parameters[2]) == 0) {
                    i = intermediate_table[i]->parameters[3] - 1;  // IF Instruction //
                    continue;
                }
            break;

			case FOR_INSTRUCTION:
				for (int j = 0; j < memory_array[intermediate_table[i]->parameters[0]]; j++) 
					asm_executor(memory_array, memory_index, i + 1, intermediate_table[i]->parameters[3] - 1, user);

				i = intermediate_table[i]->parameters[3] - 1;
			break;

			case WHILE_INSTRUCTION:
				while (check_condition(memory_array[intermediate_table[i]->parameters[0]], memory_array[intermediate_table[i]->parameters[1]],
                    intermediate_table[i]->parameters[2]) != 0) 
					asm_executor(memory_array, memory_index, i + 1, intermediate_table[i]->parameters[3] - 1, user);

				i = intermediate_table[i]->parameters[3] - 1;
			break;

			case CLEAR_INSTRUCTION:
				VGA_text_clrscr();
			break;

			case JMP_INSTRUCTION:  
                i = intermediate_table[i]->parameters[0] - 1; // JUMP or ELSE Instruction //
                continue;
            break;
		}

		i++;
	}
	return;
}