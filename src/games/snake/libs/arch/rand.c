#include "../include/rand.h"


int rand_r(int seed) {
	seed = seed * 1103515245 + 12345;
	return ((srand_r() * seed) & RAND_MAX);
}

int global_seed = 1;

int srand_r() {
    datetime_read_rtc();

	int seed = datetime_second * datetime_hour * 11015245 + 12345;
	return ((++global_seed * seed) & RAND_MAX);
}

static int rand(unsigned long *ctx) {
	long hi, lo, x;

	hi = *ctx / 127773;
	lo = *ctx % 127773;
	x = 16807 * lo - 2836 * hi;

	if (x < 0)
		x += 0x7fffffff;

	*ctx = x;
    
	return (x - 1);
}