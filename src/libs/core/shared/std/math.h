#pragma once

#define min(a,b)    ((a) < (b) ? (a) : (b))
#define max(a,b)    ((a) > (b) ? (a) : (b))

// declaration of constants.
#define PI          3.141592653589793
#define HALF_PI     1.570796326794897
#define DOUBLE_PI   6.283185307179586
#define SIN_CURVE_A 0.0415896
#define SIN_CURVE_B 0.00129810625032

//structure of a two-dimensional vector of integers.
struct vec2i {
	int x, y;
};

//structure of a three-dimensional vector of integers.
struct vec3i {
	int x, y;
};

//structure of a two-dimensional vector of rational numbers.
struct vec2f {
	float x, y;
};

//structure of a three-dimensional vector of rational numbers.
struct vec3f {
	float x, y;
};

//function to find the absolute value of an integer.
//#define abs(x) (if(x<0) -x; x)
int abs(int x);

int sgn(int x);

float sqrt(float number);

//function to compute the power of a given base with a given exponent.
int pow(int base, int exponent);

//function for calculating the cos.
double cos(double x);

//sin calculation function.
double sin(double x);