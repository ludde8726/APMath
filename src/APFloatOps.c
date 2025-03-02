#include <stdint.h>

#include "APCtx.h"
#include "APFloatOps.h"
#include "APIntOps.h"
#include "APNumber.h"

void apfloat_align(APFloat *x, APFloat *y) {
    if (x->exponent > y->exponent) {
        uint32_t needed_shift = x->exponent - y->exponent;
        uint32_t available_left_space = x->significand->capacity - x->significand->size;
        uint32_t left_shift = (needed_shift > available_left_space) ? available_left_space : needed_shift;
        if (left_shift) apint_left_shift_inplace(x->significand, left_shift);
        x->exponent -= left_shift;
        needed_shift -= left_shift;
        if (needed_shift > 0) {
            apint_right_shift_inplace(y->significand, needed_shift);
            y->exponent += needed_shift;
        }
    } else if (y->exponent > x->exponent) {
        uint32_t needed_shift = y->exponent - x->exponent;
        uint32_t available_left_space = y->significand->capacity - y->significand->size;
        uint32_t left_shift = (needed_shift > available_left_space) ? available_left_space : needed_shift;
        if (left_shift) apint_left_shift_inplace(y->significand, left_shift);
        y->exponent -= left_shift;
        needed_shift -= left_shift;
        if (needed_shift > 0) {
            apint_right_shift_inplace(x->significand, needed_shift);
            x->exponent += needed_shift;
        }
    }
}

// Addition
#if 1

APFloat *apfloat_add(APFloat *x, APFloat *y) {
    uint32_t workprec = ctx.precision + 1;
    // They always need to be resized to exactly the precision to not lose any significant digits
    apfloat_resize(x, workprec-1);
    apfloat_resize(y, workprec-1); 
    APFloat *res = apfloat_add_ex(x, y, workprec);
    apfloat_resize(res, ctx.precision);
    return res;
}

APFloat *apfloat_add_ex(APFloat *x, APFloat *y, uint32_t precision) {
    if (x->sign != y->sign) {
        if (x->sign == 1) { // y = -1
            APFloat *pos_y = apfloat_copy_ex(y, precision);
            pos_y->sign = 1;
            APFloat *res = apfloat_sub_ex(x, pos_y, precision);
            apfloat_free(pos_y);
            return res;
        } else { // x = -1
            APFloat *pos_x = apfloat_copy_ex(x, precision);
            pos_x->sign = 1;
            APFloat *res = apfloat_sub_ex(y, pos_x, precision);
            apfloat_free(pos_x);
            return res;
        }
    }
    apfloat_align(x, y);
    APInt *res_significand = apint_add_ex(x->significand, y->significand, precision);
    APFloat *res = apfloat_from_apint_ex(res_significand, x->exponent, precision);
    res->sign = x->sign;
    apfloat_normalize(res);
    return res;
}

#endif

// Subtraction
#if 1

APFloat *apfloat_sub(APFloat *x, APFloat *y) {
    uint32_t workprec = ctx.precision + 1;
    // They always need to be resized to exactly the precision to not lose any significant digits
    apfloat_resize(x, workprec-1);
    apfloat_resize(y, workprec-1); 
    APFloat *res = apfloat_sub_ex(x, y, workprec);
    apfloat_resize(res, ctx.precision);
    return res;
}

APFloat *apfloat_sub_ex(APFloat *x, APFloat *y, uint32_t precision) {
    if (x->sign != y->sign) {
        // -x - +y -> -x + y
        // x - -y -> x + y
        APFloat *neg_y = apfloat_copy(y);
        neg_y->sign = -y->sign;
        APFloat *res = apfloat_add_ex(x, neg_y, precision);
        apfloat_free(neg_y);
        return res;
    }
    apfloat_align(x, y);

    int cmp = apint_abs_compare(x->significand, y->significand);

    if (cmp == 0) {
        APFloat *zero = apfloat_init();
        zero->significand->size = 1;
        return zero;
    }
    
    const APFloat *larger = x, *smaller = y;
    int8_t result_sign = x->sign;
    if (cmp < 0) {
        larger = y;
        smaller = x;
        result_sign = -result_sign;
    }
    APInt *res_significand = apint_sub_ex(larger->significand, smaller->significand, precision);
    APFloat *res = apfloat_from_apint_ex(res_significand, x->exponent, precision);
    res->sign = result_sign;
    apfloat_normalize(res);
    return res;
}

#endif

#if 1

APFloat *apfloat_mul(APFloat *x, APFloat *y) {
    // For multiplication n + 1 significant digits is needed for the operands to achive correct result for n digits of precision
    uint32_t workprec = ctx.precision * 2 + 2; 
    if (x->significand->size > ctx.precision + 1) apfloat_resize(x, ctx.precision + 1);
    if (y->significand->size > ctx.precision + 1) apfloat_resize(y, ctx.precision + 1);
    APFloat *res = apfloat_mul_ex(x, y, workprec);
    apfloat_resize(res, ctx.precision);
    return res;
}

APFloat *apfloat_mul_ex(APFloat *x, APFloat *y, uint32_t precision) {
    APInt *res_significand = apint_mul_ex(x->significand, y->significand, precision);
    APFloat *res = apfloat_from_apint_ex(res_significand, x->exponent + y->exponent, precision);
    res->sign = x->sign == y->sign ? 1 : -1;
    apfloat_normalize(res);
    return res;
}

#endif

APFloat *apfloat_div(APFloat *x, APFloat *y);
APFloat *apfloat_div_ex(APFloat *x, APFloat *y, uint32_t precision);
APFloat *apfloat_pow(APFloat *x, APFloat *y);
APFloat *apfloat_pow_ex(APFloat *x, APFloat *y, uint32_t precision);
APFloat *apfloat_log(APFloat *x, APFloat *base);
APFloat *apfloat_log_ex(APFloat *x, APFloat *base);