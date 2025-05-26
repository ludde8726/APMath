#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "APNumber.h"
#include "APIntOps.h"
#include "test_apint_ops.h"
#include "APCtx.h"

#define RANDOM_NUMBER (long long)(rand() % RANDOM_MAX_INT) - RANDOM_MAX_INT/2

int apint_is_equal(APInt *num, long long int_num) {
    if (num->size != (int_num == 0 ? 1 : (uint32_t)log10(llabs(int_num)) + 1)) return 0;
    if (num->sign != (int_num >= 0 ? 1 : -1)) return 0;

    for (uint32_t j = 0; j < num->size; j++) {
        uint32_t num_at_j = (uint8_t)llabs((int_num % (long long)pow(10, j + 1)) / (long long)pow(10, j));
        if (num->digits[j] != num_at_j) {
            return 0;
        }
    }
    return 1;
}

int apint_test_create_from_int(void) {
    for (int i = 0; i < ITERATIONS; i++) {
        long long x = RANDOM_NUMBER;
        APInt *apint_repr = apint_from_int(x);
        if (!apint_is_equal(apint_repr, x)) return 0;
        apint_free(apint_repr);
    }
    return 1;
}

int apint_test_add(void) {
    for (int i = 0; i < ITERATIONS; i++) {
        long long x = RANDOM_NUMBER;
        long long y = RANDOM_NUMBER;
        long long res = x + y;
        APInt *apint_x = apint_from_int(x);
        APInt *apint_y = apint_from_int(y);
        APInt *apint_res = apint_add(apint_x, apint_y);

        if (!apint_is_equal(apint_res, res)) return 0;
        apint_free(apint_x);
        apint_free(apint_y);
        apint_free(apint_res);
    }
    return 1;
}
int apint_test_sub(void) {
    for (int i = 0; i < ITERATIONS; i++) {
        long long x = RANDOM_NUMBER;
        long long y = RANDOM_NUMBER;
        long long res = x - y;
        APInt *apint_x = apint_from_int(x);
        APInt *apint_y = apint_from_int(y);
        APInt *apint_res = apint_sub(apint_x, apint_y);

        if (!apint_is_equal(apint_res, res)) return 0;

        apint_free(apint_x);
        apint_free(apint_y);
        apint_free(apint_res);
    }
    return 1;
}

int apint_test_mul(void) {
    for (int i = 0; i < ITERATIONS; i++) {
        long long x = RANDOM_NUMBER;
        long long y = RANDOM_NUMBER;
        long long res = x * y;
        APInt *apint_x = apint_from_int(x);
        APInt *apint_y = apint_from_int(y);
        APInt *apint_res = apint_mul(apint_x, apint_y);

        if (!apint_is_equal(apint_res, res)) return 0;

        apint_free(apint_x);
        apint_free(apint_y);
        apint_free(apint_res);
    }
    return 1;
}

int apint_test_div(void) {
    for (int i = 0; i < ITERATIONS; i++) {
        int x = RANDOM_NUMBER;
        int y = RANDOM_NUMBER;
        if (x == 0) x+=1;
        if (y == 0) y+=1;
        int res = x / y;
        APInt *apint_x = apint_from_int(x);
        APInt *apint_y = apint_from_int(y);
        APInt *apint_res = apint_div(apint_x, apint_y, NULL);

        if (!apint_is_equal(apint_res, res)) return 0;

        apint_free(apint_x);
        apint_free(apint_y);
        apint_free(apint_res);
    }
    return 1;
}

int apint_test_pow(void) {
    for (int i = 0; i < ITERATIONS; i++) {
        long long x = (long long)(rand() % 20) - 10;
        long long y = (long long)(rand() % 10);
        x = 2;
        y = 2;
        long long res = (long long)pow(x, y);
        APInt *apint_x = apint_from_int(x);
        APInt *apint_y = apint_from_int(y);
        APInt *apint_res = apint_pow(apint_x, apint_y);

        if (!apint_is_equal(apint_res, res)) return 0;

        apint_free(apint_x);
        apint_free(apint_y);
        apint_free(apint_res);
    }
    return 1;
}
