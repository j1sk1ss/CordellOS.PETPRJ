#pragma once

#define min(a,b)    ((a) < (b) ? (a) : (b))
#define max(a,b)    ((a) > (b) ? (a) : (b))

// declaration of constants.
#define PI          3.141592653589793
#define HALF_PI     1.570796326794897
#define DOUBLE_PI   6.283185307179586
#define SIN_CURVE_A 0.0415896
#define SIN_CURVE_B 0.00129810625032

//function to find the absolute value of an integer.
//#define abs(x) (if(x<0) -x; x)
int abs(int x);

int sgn(int x);

float sqrt(float number);
double pow(double base, int exponent);

//function for calculating the cos.
double cos(double x);

//sin calculation function.
double sin(double x);