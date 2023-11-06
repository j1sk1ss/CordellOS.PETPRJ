#include "../../include/rand.h"


int rand_r(int seed) {
	seed = seed * 1103515245 + 12345;
	return (seed & RAND_MAX);
}

int srand_r() {
    datetime_read_rtc();

	int seed = datetime_second * datetime_hour * 1103515245 + 12345;
	return (seed & RAND_MAX);
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