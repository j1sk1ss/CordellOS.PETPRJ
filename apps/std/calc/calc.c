#include <stdio.h>
#include <keyboard.h>
#include <string.h>
#include <math.h>


int main(int argc, char* argv[]) {
    int interface = argc != 0 ? 0 : 1;
    int result    = -1;

    if (interface != 1) {
        int operand1 = atoi(argv[0]);
        char op      = argv[1][0];
        int operand2 = atoi(argv[2]);

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

            case '^':
                result = pow(operand1, operand2);
            break;

            case '/':
                if (operand2 != 0) result = operand1 / operand2;
            break;
        }
    }

    return result;
}