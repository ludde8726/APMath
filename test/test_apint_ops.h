#define RANDOM_MAX_INT 999
#define ITERATIONS 100000

#define RANDOM_NUMBER (long long)(rand() % RANDOM_MAX_INT) - RANDOM_MAX_INT/2

int test_create_from_int();
int test_add();
int test_sub();
int test_mul();