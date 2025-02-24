#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "APNumber.h"
#include "APIntOps.h"
#include "test_apint_ops.h"

#define RANDOM_NUMBER (long long)(rand() % RANDOM_MAX_INT) - RANDOM_MAX_INT/2

int test_create_from_int() {
    for (int i = 0; i < ITERATIONS; i++) {
        int x = RANDOM_NUMBER;
        uint32_t intlen = x == 0 ? 1 : (uint32_t)log10(abs(x)) + 1;
        APInt *apint_repr = apint_from_int(x);
        if (apint_repr->size != intlen) return 0;
        if (apint_repr->sign != (x >= 0 ? 1 : -1)) return 0;
        for (uint32_t j = 0; j < intlen; j++) {
            uint32_t num_at_j = (uint8_t)abs((x % (int)pow(10, j + 1)) / (int)pow(10, j));
            if (apint_repr->digits[j] != num_at_j) return 0;
        }
        apint_free(apint_repr);
    }
    return 1;
}

int test_add() {
    for (int i = 0; i < ITERATIONS; i++) {
        int x = RANDOM_NUMBER;
        int y = RANDOM_NUMBER;
        int res = x + y;
        APInt *apint_x = apint_from_int(x);
        APInt *apint_y = apint_from_int(y);
        APInt *apint_res = apint_add(apint_x, apint_y);

        if (apint_res->size != (res == 0 ? 1 : (uint32_t)log10(abs(res)) + 1)) return 0;
        if (apint_res->sign != (res >= 0 ? 1 : -1)) return 0;
        for (uint32_t j = 0; j < apint_res->size; j++) {
            uint32_t num_at_j = (uint8_t)abs((res % (int)pow(10, j + 1)) / (int)pow(10, j));
            if (apint_res->digits[j] != num_at_j) return 0;
        }
        apint_free(apint_x);
        apint_free(apint_y);
        apint_free(apint_res);
    }
    return 1;
}
int test_sub() {
    for (int i = 0; i < ITERATIONS; i++) {
        int x = RANDOM_NUMBER;
        int y = RANDOM_NUMBER;
        int res = x - y;
        APInt *apint_x = apint_from_int(x);
        APInt *apint_y = apint_from_int(y);
        APInt *apint_res = apint_sub(apint_x, apint_y);

        if (apint_res->size != (res == 0 ? 1 : (uint32_t)log10(abs(res)) + 1)) return 0;
        if (apint_res->sign != (res >= 0 ? 1 : -1)) return 0;

        for (uint32_t j = 0; j < apint_res->size; j++) {
            uint32_t num_at_j = (uint8_t)abs((res % (int)pow(10, j + 1)) / (int)pow(10, j));
            if (apint_res->digits[j] != num_at_j) return 0;
        }
        apint_free(apint_x);
        apint_free(apint_y);
        apint_free(apint_res);
    }
    return 1;
}

int test_mul() {
    for (int i = 0; i < ITERATIONS; i++) {
        int x = RANDOM_NUMBER;
        int y = RANDOM_NUMBER;
        int res = x * y;
        APInt *apint_x = apint_from_int(x);
        APInt *apint_y = apint_from_int(y);
        APInt *apint_res = apint_mul(apint_x, apint_y);

        if (apint_res->size != (res == 0 ? 1 : (uint32_t)log10(abs(res)) + 1)) return 0;
        if (apint_res->sign != (res >= 0 ? 1 : -1)) return 0;

        for (uint32_t j = 0; j < apint_res->size; j++) {
            uint32_t num_at_j = (uint8_t)abs((res % (int)pow(10, j + 1)) / (int)pow(10, j));
            if (apint_res->digits[j] != num_at_j) return 0;
        }

        apint_free(apint_x);
        apint_free(apint_y);
        apint_free(apint_res);
    }
    return 1;
}
