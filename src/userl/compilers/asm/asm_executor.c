#include "../../include/asm_compiler.h"


extern int symbol_index;
extern int intermediate_index;
extern int blocks_index;
extern struct intermediate_lang **intermediate_table;
extern struct symbol_table **symbol_tab;
extern struct blocks_table **block_tab;

void display_symbol_table() {
	kprintf("\n---------------Symbol Table is ----------\n");
	for (int i = 0; i < symbol_index; i++)
		kprintf("\nvariable name is %s,address is %d, size is %d", symbol_tab[i]->variable_name, symbol_tab[i]->address, symbol_tab[i]->size);

	return;
}

void display_intermediate_table() {
	kprintf("\n---------------Instruction Table is ----------\n");
	for (int i = 0; i < intermediate_index; i++){
		kprintf("\n%d : %d : ", intermediate_table[i]->instruction_position, intermediate_table[i]->opcode);
		for (int j = 0; intermediate_table[i]->parameters[j] != -1; j++)
			kprintf(" %d", intermediate_table[i]->parameters[j]);
	}

	kprintf("\n");
	return;
}

void display_block_table() {
	kprintf("\n---------------Block Table is ----------\n");
	for (int i = 0; i < blocks_index; i++)
		kprintf("\n Label is : %s and address is : %d ", block_tab[i]->name, block_tab[i]->instr_no);

	kprintf("\n");
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
				kprintf("\n\r");
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
                kprintf("\n%d\n", memory_array[intermediate_table[i]->parameters[0]]);
	    	break;  // PRINT Instruction //

			case PRINTS_INSTRUCTION: 
                kprintf("%s", intermediate_table[i]->string);
	    	break;  // PRINTS Instruction //

			case PRINTL_INSTRUCTION:
                kprintf("\n%s", intermediate_table[i]->string);
	    	break;  // PRINTL Instruction //

			case MKFILE_INSTRUCTION:
				struct FATContent* content = FAT_create_content(intermediate_table[i]->string_params[0], FALSE, "txt");
				FAT_put_content(intermediate_table[i]->string_params[1], content);
				FAT_unload_content_system(content);
			break;

			case RMFILE_INSTRUCTION:
				if (FAT_content_exists(intermediate_table[i]->string) == 1)
					FAT_delete_content(intermediate_table[i]->string_params[1], intermediate_table[i]->string_params[0]);
			break;

			case WFILE_INSTRUCTION:
				if (FAT_content_exists(intermediate_table[i]->string_params[0]) == 1)
						FAT_edit_content(intermediate_table[i]->string_params[0], intermediate_table[i]->string_params[1]);
			break;

			case RFILE_INSTRUCTION:
				if (FAT_content_exists(intermediate_table[i]->string_params[0]) == 1) {
					struct FATContent* content = FAT_get_content(intermediate_table[i]->string_params[0]);

					char* fat_data = FAT_read_content(content);
					int data = atoi(fat_data);
					free(fat_data);

					memory_array[intermediate_table[i]->parameters[0]] = data; 

					FAT_unload_content_system(content);
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