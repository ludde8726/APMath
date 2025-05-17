#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "APCtx.h"
#include "APIntOps.h"
#include "APFloatOps.h"
#include "APNumber.h"
#include "APHelpers.h"
#include "APError.h"


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
    APF_DIG(small_float)[0] = 3;
    APF_DIG(small_float)[1] = 2;
    APF_SZ(small_float) = 2;
    small_float->exponent = -2;

    APF_DIG(small_float2)[0] = 2;
    APF_SZ(small_float2) = 1;
    small_float2->exponent = 0;

    small_int2->digits[0] = 3;
    small_int2->size = 1;
    small_int2->sign = -1;

    print_apfloat(small_float, REGULAR);
    print_apfloat(small_float2, REGULAR);

    // APFloat *res = apfloat_pow(small_float, small_int2);
    // print_apfloat(res, REGULAR);

    apfloat_free(small_float);
    apfloat_free(small_float2);
    // apfloat_free(res);

    APFloat *numes = apfloat_init();
    APF_DIG(numes)[0] = 3;
    APF_SZ(numes) = 1;
    numes->exponent = 10;
    DEBUG_PRINT_APINT(numes->significand);
    print_apfloat(numes, SCIENTIFIC);
    apfloat_free(numes);

    APInt *sint = apint_init();
    sint->digits[0] = 4;
    sint->size = 1;

    APInt *sint2 = apint_init();
    sint2->digits[2] = 1;
    sint2->digits[3] = 7;
    sint2->digits[4] = 7;
    sint2->digits[5] = 7;
    sint2->digits[6] = 7;
    sint2->size = 7;
    // DEBUG_PRINT_APINT(sint2);

    APInt *resa = apint_pow(sint2, sint);
    DEBUG_PRINT_APINT(resa);
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
        fflush(stdout);
    }
    printf("\n");
    if (failed > 0) printf("Failed %d test(s)! Out of %d total.\n", failed, num_tests);
    else printf("All tests (%d/%d) passed!\n", num_tests, num_tests);
}

int main() {
    srand(time(NULL));
    TestFunction tests[] = { apint_test_create_from_int, apint_test_add, apint_test_sub, 
                             apint_test_mul, apint_test_div, apfloat_test_add, 
                             apfloat_test_sub, apfloat_test_mul, apfloat_test_div };
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    test_runner(tests, num_tests);
    printf("\n");
    testing();
    return 0;
}