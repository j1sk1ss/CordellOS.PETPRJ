#include "../../../libs/include/syscall.h"

#define F3_BUTTON               '\255'
#define ENTER_BUTTON            '\n'

int main(int args, char** argv) {
    while (1) {
        char message[35] = "Enter expression (e.g., 5 + 3): ";
        SYS_puts(message);

        int operand1 = 0;
        while (1) {
            char key = SYS_keyboard_wait_key();
            if (key >= '0' && key <= '9') {
                operand1 = operand1 * 10 + (key - '0');
                SYS_putc(key);
            } else if (key == ' ') break;
        }

        char op = SYS_keyboard_wait_key();
        SYS_putc(op);

        int operand2 = 0;
        while (1) {
            char key = SYS_keyboard_wait_key();
            if (key >= '0' && key <= '9') {
                operand2 = operand2 * 10 + (key - '0');
                SYS_putc(key);
            } else if (key == '=') break;
        }

        int result;
        switch (op) {
            case '+':
                result = operand1 + operand2;
            break;

            case '-':
                result = operand1 - operand2;
            break;

            case '*':
                result = operand1 * operand2;
            break;

            case '/':
                if (operand2 != 0) result = operand1 / operand2;
                else {
                    char error[26] = "Error: Division by zero\n";
                    SYS_puts(error);
                    continue;
                }
            break;

            default:
                char warn[26] = "Error: Invalid operator\n";
                SYS_puts(warn);
            continue;
        }

        char eq[4] = " = ";
        SYS_puts(eq);
        
        char result_buffer[20];
        int_to_string(result, result_buffer, sizeof(result_buffer));
        SYS_puts(result_buffer);
        SYS_putc('\n');

        char exit_message[40] = "Press F3 to exit or ENTER to continue\n";
        SYS_puts(exit_message);
        while (1) {
            switch (SYS_keyboard_wait_key()) {
                case F3_BUTTON:
                return -1;

                case ENTER_BUTTON:
                break;

                default:
                continue;
            }
        }

        SYS_clrs();
    }

    return 0;
}

void int_to_string(int num, char* buffer, int buffer_size) {
    int i = 0;
    int temp = num;
    
    // Handle the case where the number is 0 separately
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    // Handle negative numbers
    if (num < 0) {
        buffer[i++] = '-';
        temp = -temp;
    }

    // Calculate the number of digits
    int digits = 0;
    while (temp > 0) {
        temp /= 10;
        digits++;
    }

    // Fill the buffer with digits
    for (int j = 0; j < digits; j++) {
        int divisor = 1;
        for (int k = 0; k < digits - j - 1; k++) {
            divisor *= 10;
        }
        buffer[i++] = '0' + ((num / divisor) % 10);
    }

    // Null-terminate the string
    buffer[i] = '\0';
}