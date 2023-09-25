#include "math.h"

double pow(double base, int exponent) {
    double result = 1.0;

    // Handle negative exponents
    if (exponent < 0) {
        base = 1.0 / base;
        exponent = -exponent;
    }

    // Calculate the power using repeated multiplication
    for (int i = 0; i < exponent; i++) 
        result *= base;
    
    return result;
}