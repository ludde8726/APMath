#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "APCtx.h"
#include "APIntOps.h"
#include "APFloatOps.h"
#include "APNumber.h"

#include "test_apint_ops.h"
#include "test_apfloat_ops.h"

typedef int (*TestFunction)();

void testing() {
    // Set precision to 5 (we don't need more)
    ctx.precision = 50;
    // Create two ints
    APFloat *small_float = apfloat_init();
    APFloat *small_float2 = apfloat_init();
    small_float->significand->digits[4] = 1;
    small_float->significand->digits[3] = 1;
    small_float->significand->digits[2] = 1;
    small_float->significand->size = 5;
    small_float2->significand->digits[4] = 9;
    small_float2->significand->digits[3] = 9;
    small_float2->significand->digits[2] = 9;
    small_float2->significand->digits[1] = 9;
    small_float2->significand->digits[0] = 9;
    small_float2->significand->size = 5;
    small_float->exponent = 1;
    small_float2->exponent = -3;
    // 11.100 + 111.00 =
    APFloat *res = apfloat_add(small_float, small_float2);
    for (uint32_t i = 0; i < res->significand->capacity; i++) printf("%d", res->significand->digits[i]);
    printf("\n");
    
    for (uint32_t i = 0; i < small_float->significand->capacity; i++) printf("%d", small_float->significand->digits[i]);
    printf("\n");
    for (uint32_t i = 0; i < small_float2->significand->capacity; i++) printf("%d", small_float2->significand->digits[i]);
    printf("\n");

    apint_resize(small_float2->significand, 4);

    for (uint32_t i = 0; i < small_float2->significand->capacity; i++) printf("%d ", small_float2->significand->digits[i]);
    printf("\n");

    printf("RES_EXP: %lld\n", res->exponent);
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
    TestFunction tests[] = { apint_test_create_from_int, apint_test_add, apint_test_sub, apint_test_mul, apint_test_div, apint_test_pow, apfloat_test_add, apfloat_test_sub };
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    test_runner(tests, num_tests);
    printf("\n");
    testing();
    return 0;
}