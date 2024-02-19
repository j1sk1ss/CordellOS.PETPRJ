#include <stdio.h>


int main(int args, char* argv[]) {
    while (1) {
        printf("Enter expression (e.g., 5 + 3): ");

        int operand1 = 0;
        while (1) {
            char key = wait_char();
            if (key >= '0' && key <= '9') {
                operand1 = operand1 * 10 + (key - '0');
                putc(key);
            } else if (key == ' ') break;
        }

        char op = wait_char();
        putc(op);

        int operand2 = 0;
        while (1) {
            char key = wait_char();
            if (key >= '0' && key <= '9') {
                operand2 = operand2 * 10 + (key - '0');
                putc(key);
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
                    printf("Error: Division by zero\n");
                    continue;
                }
            break;

            default:
                printf("Error: Invalid operator\n");
            continue;
        }

        char eq[4] = " = ";
        printf(eq);
        
        char result_buffer[20];
        int_to_string(result, result_buffer, sizeof(result_buffer));
        printf(result_buffer);
        putc('\n');

        result = 0;
        printf("Press F3 to exit or ENTER to continue\n");
        while (result == 0) {
            switch (wait_char()) {
                case F3_BUTTON:
                return -1;

                case ENTER_BUTTON:
                    result = 1;
                break;

                default:
                continue;
            }
        }

        clrscr();
    }

    return 0;
}

void int_to_string(int num, char* buffer, int buffer_size) {
    int i    = 0;
    int temp = num;
    
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    if (num < 0) {
        buffer[i++] = '-';
        temp = -temp;
    }

    int digits = 0;
    while (temp > 0) {
        temp /= 10;
        digits++;
    }

    for (int j = 0; j < digits; j++) {
        int divisor = 1;
        for (int k = 0; k < digits - j - 1; k++) 
            divisor *= 10;

        buffer[i++] = '0' + ((num / divisor) % 10);
    }

    buffer[i] = '\0';
}