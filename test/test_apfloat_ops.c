#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "APFloatOps.h"
#include "APNumber.h"
#include "APCtx.h"
#include "APHelpers.h"

#include "test_apfloat_ops.h"
#include "test_apint_ops.h"


int apfloat_almost_eq(APFloat *num, long double float_num) {
    int float_sign = float_num < 0 ? -1 : 1;
    if (float_sign != num->sign) return 0;
    long long num_as_int = 0;
    for (uint32_t i = 0; i < num->significand->size; i++) num_as_int += (long long)num->significand->digits[i] * (long long)pow(10, i);
    long double num_as_float = num->sign * (long double)num_as_int * powl(10, num->exponent);
    return fabsl(float_num - num_as_float) < powl(10, num->exponent+1);
}

int apfloat_test_add() {
    ctx.precision = PRECISION;
    for (int i = 0; i < ITERATIONS; i++) {
        long long x_man = (long long)(rand() % (long long)(pow(10, PRECISION)-1)) - (long long)(pow(10, PRECISION)-1) / 2;
        long long y_man = (long long)(rand() % (long long)(pow(10, PRECISION)-1)) - (long long)(pow(10, PRECISION)-1) / 2;

        long long x_exponent = (long long)(rand() % PRECISION) - PRECISION/2;
        long long y_exponent = (long long)(rand() % PRECISION) - PRECISION/2;

        long double real_x = x_man * pow(10, x_exponent);
        long double real_y = y_man * pow(10, y_exponent);
        long double real_res = real_x + real_y;
        
        APInt *apfloat_x_significand = apint_from_int(x_man);
        APInt *apfloat_y_significand = apint_from_int(y_man);

        APFloat *apfloat_x = apfloat_from_apint(apfloat_x_significand, x_exponent);
        APFloat *apfloat_y = apfloat_from_apint(apfloat_y_significand, y_exponent);

        APFloat *apfloat_res = apfloat_add(apfloat_x, apfloat_y);

        if (!apfloat_almost_eq(apfloat_res, real_res)) return 0;

        apfloat_free(apfloat_x);
        apfloat_free(apfloat_y);
        apfloat_free(apfloat_res);
    }
    return 1;
}

int apfloat_test_sub() {
    ctx.precision = PRECISION;
    for (int i = 0; i < ITERATIONS; i++) {
        long long x_man = (long long)(rand() % (long long)(pow(10, PRECISION)-1)) - (long long)(pow(10, PRECISION)-1) / 2;
        long long y_man = (long long)(rand() % (long long)(pow(10, PRECISION)-1)) - (long long)(pow(10, PRECISION)-1) / 2;

        long long x_exponent = (long long)(rand() % PRECISION) - PRECISION/2;
        long long y_exponent = (long long)(rand() % PRECISION) - PRECISION/2;

        long double real_x = x_man * pow(10, x_exponent);
        long double real_y = y_man * pow(10, y_exponent);
        long double real_res = real_x - real_y;
        
        APInt *apfloat_x_significand = apint_from_int(x_man);
        APInt *apfloat_y_significand = apint_from_int(y_man);

        APFloat *apfloat_x = apfloat_from_apint(apfloat_x_significand, x_exponent);
        APFloat *apfloat_y = apfloat_from_apint(apfloat_y_significand, y_exponent);

        APFloat *apfloat_res = apfloat_sub(apfloat_x, apfloat_y);

        if (!apfloat_almost_eq(apfloat_res, real_res)) return 0;

        apfloat_free(apfloat_x);
        apfloat_free(apfloat_y);
        apfloat_free(apfloat_res);
    }
    return 1;
}

int apfloat_test_mul() {
    ctx.precision = PRECISION;
    for (int i = 0; i < ITERATIONS; i++) {
        long long x_man = (long long)(rand() % (long long)(pow(10, PRECISION)-1)) - (long long)(pow(10, PRECISION)-1) / 2;
        long long y_man = (long long)(rand() % (long long)(pow(10, PRECISION)-1)) - (long long)(pow(10, PRECISION)-1) / 2;

        long long x_exponent = (long long)(rand() % PRECISION) - PRECISION/2;
        long long y_exponent = (long long)(rand() % PRECISION) - PRECISION/2;

        long double real_x = x_man * pow(10, x_exponent);
        long double real_y = y_man * pow(10, y_exponent);
        long double real_res = real_x * real_y;
        
        APInt *apfloat_x_significand = apint_from_int(x_man);
        APInt *apfloat_y_significand = apint_from_int(y_man);

        APFloat *apfloat_x = apfloat_from_apint(apfloat_x_significand, x_exponent);
        APFloat *apfloat_y = apfloat_from_apint(apfloat_y_significand, y_exponent);

        APFloat *apfloat_res = apfloat_mul(apfloat_x, apfloat_y);

        if (!apfloat_almost_eq(apfloat_res, real_res)) return 0;

        apfloat_free(apfloat_x);
        apfloat_free(apfloat_y);
        apfloat_free(apfloat_res);
    }
    return 1;
}

int apfloat_test_div() {
    ctx.precision = PRECISION;
    for (int i = 0; i < ITERATIONS; i++) {
        long long x_man = (long long)(rand() % (long long)(pow(10, PRECISION)-1)) - (long long)(pow(10, PRECISION)-1) / 2;
        long long y_man = (long long)(rand() % (long long)(pow(10, PRECISION)-1)) - (long long)(pow(10, PRECISION)-1) / 2;

        long long x_exponent = (long long)(rand() % PRECISION) - PRECISION/2;
        long long y_exponent = (long long)(rand() % PRECISION) - PRECISION/2;

        long double real_x = x_man * pow(10, x_exponent);
        long double real_y = y_man * pow(10, y_exponent);
        long double real_res = real_x / real_y;
        
        APInt *apfloat_x_significand = apint_from_int(x_man);
        APInt *apfloat_y_significand = apint_from_int(y_man);

        APFloat *apfloat_x = apfloat_from_apint(apfloat_x_significand, x_exponent);
        APFloat *apfloat_y = apfloat_from_apint(apfloat_y_significand, y_exponent);

        APFloat *apfloat_res = apfloat_div(apfloat_x, apfloat_y);

        if (!apfloat_almost_eq(apfloat_res, real_res)) return 0;

        apfloat_free(apfloat_x);
        apfloat_free(apfloat_y);
        apfloat_free(apfloat_res);
    }
    return 1;
}