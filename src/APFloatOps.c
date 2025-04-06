#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "APCtx.h"
#include "APFloatOps.h"
#include "APIntOps.h"
#include "APNumber.h"
#include "APHelpers.h"

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
    APFloat *res = apfloat_add_impl(x, y, workprec);
    apfloat_round_resize(res, ctx.precision);
    return res;
}

APFloat *apfloat_add_ex(APFloat *x, APFloat *y, uint32_t precision) {
    uint32_t workprec = precision + 1;
    apfloat_resize(x, workprec-1);
    apfloat_resize(y, workprec-1); 
    APFloat *res = apfloat_add_impl(x, y, workprec);
    apfloat_round_resize(res, precision);
    return res;
}

APFloat *apfloat_add_impl(APFloat *x, APFloat *y, uint32_t precision)
{
    if (x->sign != y->sign) {
        if (x->sign == 1) { // y = -1
            APFloat *pos_y = apfloat_copy_ex(y, precision);
            pos_y->sign = 1;
            APFloat *res = apfloat_sub_impl(x, pos_y, precision);
            apfloat_free(pos_y);
            return res;
        } else { // x = -1
            APFloat *pos_x = apfloat_copy_ex(x, precision);
            pos_x->sign = 1;
            APFloat *res = apfloat_sub_impl(y, pos_x, precision);
            apfloat_free(pos_x);
            return res;
        }
    }
    apfloat_align(x, y);
    APInt *res_significand = apint_add_impl(x->significand, y->significand, precision);
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
    APFloat *res = apfloat_sub_impl(x, y, workprec);
    apfloat_round_resize(res, ctx.precision);
    return res;
}

APFloat *apfloat_sub_ex(APFloat *x, APFloat *y, uint32_t precision) {
    uint32_t workprec = precision + 1;
    apfloat_resize(x, workprec-1);
    apfloat_resize(y, workprec-1); 
    APFloat *res = apfloat_sub_impl(x, y, workprec);
    apfloat_round_resize(res, precision);
    return res;
}

APFloat *apfloat_sub_impl(APFloat *x, APFloat *y, uint32_t precision)
{
    if (x->sign != y->sign) {
        // -x - +y -> -x + y
        // x - -y -> x + y
        APFloat *neg_y = apfloat_copy(y);
        neg_y->sign = -y->sign;
        APFloat *res = apfloat_add_impl(x, neg_y, precision);
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
    APInt *res_significand = apint_sub_impl(larger->significand, smaller->significand, precision);
    APFloat *res = apfloat_from_apint_ex(res_significand, x->exponent, precision);
    res->sign = result_sign;
    apfloat_normalize(res);
    return res;
}

#endif

// Multiplication
#if 1

APFloat *apfloat_mul(APFloat *x, APFloat *y) {
    // For multiplication n + 1 significant digits is needed for the operands to achive correct result for n digits of precision
    uint32_t workprec = ctx.precision * 2 + 2; 
    if (x->significand->size > ctx.precision + 1) apfloat_resize(x, ctx.precision + 1);
    if (y->significand->size > ctx.precision + 1) apfloat_resize(y, ctx.precision + 1);
    APFloat *res = apfloat_mul_impl(x, y, workprec);
    apfloat_round_resize(res, ctx.precision);
    return res;
}

APFloat *apfloat_mul_ex(APFloat *x, APFloat *y, uint32_t precision) {
    uint32_t workprec = precision * 2 + 2; 
    if (x->significand->size > ctx.precision + 1) apfloat_resize(x, ctx.precision + 1);
    if (y->significand->size > ctx.precision + 1) apfloat_resize(y, ctx.precision + 1);
    APFloat *res = apfloat_mul_impl(x, y, workprec);
    apfloat_round_resize(res, precision);
    return res;
}

APFloat *apfloat_mul_impl(APFloat *x, APFloat *y, uint32_t precision) {
    APInt *res_significand = apint_mul_impl(x->significand, y->significand, precision);
    APFloat *res = apfloat_from_apint_ex(res_significand, x->exponent + y->exponent, precision);
    res->sign = x->sign == y->sign ? 1 : -1;
    apfloat_normalize(res);
    return res;
}

#endif

// Division
#if 1

APFloat *apfloat_div(APFloat *x, APFloat *y) {
    uint32_t workprec = ctx.precision + 2;

    // For division to be correct we need at least n+1 (significant) digits while also keeping the magnitude,
    // the magnitude is stored by increasing the exponent by the amount the size lowers which is done automatically
    // inside of apfloat_resize
    // However because of the poor implementation we need at least n+2 (significant) digits when dividing.
    apfloat_resize(x, workprec);
    apfloat_resize(y, workprec);
    APFloat *res = apfloat_div_impl(x, y, workprec);
    apfloat_round_resize(res, ctx.precision);
    return res;
}

APFloat *apfloat_div_ex(APFloat *x, APFloat *y, uint32_t precision) {
    uint32_t workprec = precision + 2;
    if (x->significand->size > workprec) apfloat_resize(x, workprec);
    if (y->significand->size > workprec) apfloat_resize(y, workprec);
    APFloat *res = apfloat_div_impl(x, y, workprec);
    apfloat_round_resize(res, precision);
    return res;
}

APFloat *apfloat_div_impl(APFloat *x, APFloat *y, uint32_t precision) {
    APInt *remainder;
    APInt *initial_significand = apint_div_impl(x->significand, y->significand, &remainder, precision);
    APFloat *res = apfloat_from_apint(initial_significand, x->exponent-y->exponent);
    bool should_shift = !apint_is_zero(initial_significand);
    while (!apint_is_zero(remainder) && res->significand->size < precision) {
        if (remainder->size < precision) apint_left_shift_inplace(remainder, 1);
        else apint_right_shift_inplace(y->significand, 1);
        DIGITS_DTYPE count = 0;
        while (apint_abs_compare(remainder, y->significand) >= 0) {
            apint_sub_inplace(remainder, y->significand);
            count++;
        }
        if (should_shift) apint_left_shift_inplace(res->significand, 1);
        if (count) should_shift = true;
        res->exponent--;
        res->significand->digits[0] = count;
    }
    apint_free(remainder);
    res->sign = x->sign == y->sign ? 1 : -1;
    apfloat_normalize(res);
    return res;
}

#endif

// Exponentiation
#if 1

APFloat *apfloat_pow(APFloat *x, APInt *y) {
    // Since we basically perform multiplications multiple times we need to set the precision for the multiplications to n + 1
    uint32_t workprec = ctx.precision + 1;
    if (x->significand->size > ctx.precision + y->size) apfloat_resize(x, ctx.precision + y->size);

    APFloat *res = apfloat_pow_impl(x, y, workprec);
    apfloat_round_resize(res, ctx.precision);
    return res;
}

APFloat *apfloat_pow_ex(APFloat *x, APInt *y, uint32_t precision) {
    uint32_t workprec = precision + 1;
    if (x->significand->size > precision + y->size) apfloat_resize(x, precision + y->size);

    APFloat *res = apfloat_pow_impl(x, y, workprec);
    apfloat_round_resize(res, precision);
    return res;
}

APFloat *apfloat_pow_impl(APFloat *x, APInt *y, uint32_t precision) {
    if (y->size > 19 || (y->size == 19 && y->digits[19] >= 9)) {
        fprintf(stderr, "Maximum exponent allowed is 8,999,999,999,999,999,999");
        exit(EXIT_FAILURE);
    }
    APInt *base = apint_copy_ex(x->significand, x->significand->size);
    APInt *exponent = apint_copy_ex(y, y->size);
    APInt *two = apint_init_ex(1);
    two->digits[0] = 2;
    two->size = 1;
    
    APInt *res = apint_init_ex(1);
    res->digits[0] = 1;
    res->size = 1;
    
    int64_t y_as_int = 0;
    for (uint32_t i = y->size; i > 0; i--) y_as_int += (int64_t)(y->digits[i-1] * powl(10, i-1));
    int64_t res_exponent = x->exponent * y_as_int;
    if (!((res_exponent < 0 && x->exponent < 0) || (res_exponent >= 0 && x->exponent >= 0))) {
        fprintf(stderr, "Exponent integer overflow!\n");
        exit(EXIT_FAILURE);
    }
    
    while (!apint_is_zero(exponent)) {
        APInt *quotient = NULL;
        APInt *remainder = NULL;
        quotient = apint_div_impl(exponent, two, &remainder, exponent->size);
        
        if (!apint_is_zero(remainder)) {
            APInt *temp = res;
            res = apint_mul_impl(res, base, res->size + base->size);
            if (res->size > precision) {
                res_exponent += (int64_t)(res->size - precision);
                apint_resize(res, precision);
            }
            apint_free(temp);
        }
        apint_free(exponent);
        apint_free(remainder);
        exponent = quotient;
        
        APInt *temp = base;
        base = apint_mul_impl(base, base, base->size * 2);
        apint_free(temp);
    }
    
    apint_free(base);
    apint_free(exponent);
    apint_free(two);
    
    apint_resize(res, res->size);
    APFloat *apfloat_repr = apfloat_from_apint(res, res_exponent);
    apfloat_normalize(apfloat_repr);
    
    if (y->sign == -1) {
        APFloat *one = apfloat_init_ex(1); 
        one->significand->digits[0] = 1;
        one->significand->size = 1;
        APFloat *res_inverse = apfloat_div_impl(one, apfloat_repr, precision);

        apfloat_free(apfloat_repr);
        apfloat_free(one);
        apfloat_normalize(res_inverse);
        return res_inverse;
    }
    
    return apfloat_repr;
}

#endif

APFloat *apfloat_log(APFloat *x, APFloat *base);
APFloat *apfloat_log_impl(APFloat *x, APFloat *base);

// APFloat *apfloat_e_ex(uint32_t precicion) {
//     // Use formula e = 2 + 1/2! + 1/3! + 1/4! ...
//     // Continue adding more terms while 1/n! is greater than 10^-(prec-1) i think
//     APFloat *res = apfloat_init_ex(precicion);
// }