#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "APCtx.h"
#include "APIntOps.h"
#include "APFloatOps.h"
#include "APNumber.h"
#include "APHelpers.h"


#include "test_apint_ops.h"
#include "test_apfloat_ops.h"

typedef int (*TestFunction)();

void testing() {
    // Set precision to 5 (we don't need more)
    ctx.precision = 5;
    // Create two ints
    APFloat *small_float = apfloat_init();
    APFloat *small_float2 = apfloat_init();
    APInt *small_int2 = apint_init();
    small_float->significand->digits[0] = 3;
    small_float->significand->digits[1] = 2;
    small_float->significand->size = 2;
    small_float->exponent = -2;

    small_float2->significand->digits[0] = 2;
    small_float2->significand->size = 1;
    small_float2->exponent = 0;

    small_int2->digits[0] = 3;
    small_int2->size = 1;
    small_int2->sign = -1;

    print_apfloat(small_float, REGULAR);
    print_apfloat(small_float2, REGULAR);

    APFloat *res = apfloat_pow(small_float, small_int2);
    print_apfloat(res, REGULAR);

    apfloat_free(small_float);
    apfloat_free(small_float2);
    apfloat_free(res);

}

void test_runner(TestFunction tests[], int num_tests) {
    int failed = 0;
    printf("Running tests...\n");
    for (int i = 0; i < num_tests; i++) {
        int res = tests[i]();
        if (!res) {
            failed++;
            printf("F");
        } else printf(".");
    }
    printf("\n");
    if (failed > 0) printf("Failed %d test(s)!\n", failed);
    else printf("All tests passed!\n");
}

int main() {
    srand(time(NULL));
    TestFunction tests[] = { apint_test_create_from_int, apint_test_add, apint_test_sub, apint_test_mul, apint_test_div, apint_test_pow, apfloat_test_add, apfloat_test_sub, apfloat_test_mul, apfloat_test_div };
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    test_runner(tests, num_tests);
    printf("\n");
    testing();
    return 0;
}