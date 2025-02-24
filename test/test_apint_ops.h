#define RANDOM_MAX_INT 999999999
#define ITERATIONS 1000

#define RANDOM_NUMBER (long long)(rand() % RANDOM_MAX_INT) - RANDOM_MAX_INT/2

int test_create_from_int();
int test_add();
int test_sub();