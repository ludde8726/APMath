#include "APNumber.h"

#define RANDOM_MAX_INT 1234567
#define ITERATIONS 100000

#define RANDOM_NUMBER (long long)(rand() % RANDOM_MAX_INT) - RANDOM_MAX_INT/2

int apint_is_equal(APInt *num, long long int_num);

int apint_test_create_from_int(void);
int apint_test_add(void);
int apint_test_sub(void);
int apint_test_mul(void);
int apint_test_div(void);
int apint_test_pow(void);
