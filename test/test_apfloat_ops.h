#include <stdint.h>

#include "APNumber.h"

#define PRECISION 6
#define RANDOM_MAX 999
#define ITERATIONS 100000
#define RANDOM_MAX_FLOAT 100

int apfloat_almost_eq(APFloat *num, long double float_num);

int apfloat_test_add();
int apfloat_test_sub();