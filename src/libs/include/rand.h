#define	RAND_MAX	2147483647

int rand_r(int seed);
static int rand(unsigned long *ctx);
int srand_r();