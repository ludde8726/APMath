#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "APCtx.h"
#include "APFloatOps.h"
#include "APIntOps.h"
#include "APNumber.h"
#include "APHelpers.h"

bool apfloat_align(APFloat *x, APFloat *y) {
    if (x->exponent > y->exponent) {
        uint32_t needed_shift = x->exponent - y->exponent;
        uint32_t available_left_space = APF_CAP(x) - APF_SZ(x);
        uint32_t left_shift = (needed_shift > available_left_space) ? available_left_space : needed_shift;
        if (left_shift) if(!apint_left_shift_inplace(x->significand, left_shift)) return false;
        x->exponent -= left_shift;
        needed_shift -= left_shift;
        if (needed_shift > 0) {
            apint_right_shift_inplace(y->significand, needed_shift);
            y->exponent += needed_shift;
        }
    } else if (y->exponent > x->exponent) {
        uint32_t needed_shift = y->exponent - x->exponent;
        uint32_t available_left_space = APF_CAP(y) - APF_SZ(y);
        uint32_t left_shift = (needed_shift > available_left_space) ? available_left_space : needed_shift;
        if (left_shift) if (!apint_left_shift_inplace(y->significand, left_shift)) return false;
        y->exponent -= left_shift;
        needed_shift -= left_shift;
        if (needed_shift > 0) {
            apint_right_shift_inplace(x->significand, needed_shift);
            x->exponent += needed_shift;
        }
    }
    return true;
}

// Addition
#if 1

APFloat *apfloat_add(APFloat *x, APFloat *y) {
    uint32_t workprec = ctx.precision + 1;
    // They always need to be resized to exactly the precision to not lose any significant digits
    if (!apfloat_resize(x, workprec-1)) return NULL;
    if (!apfloat_resize(y, workprec-1)) return NULL; 
    APFloat *res = apfloat_add_impl(x, y, workprec);
    if (!res) return NULL;
    if (!apfloat_round_resize(res, ctx.precision)) {
        apfloat_free(res);
        return NULL;
    }
    return res;
}

APFloat *apfloat_add_ex(APFloat *x, APFloat *y, uint32_t precision) {
    uint32_t workprec = precision + 1;
    if (!apfloat_resize(x, workprec-1)) return NULL;
    if (!apfloat_resize(y, workprec-1)) return NULL; 
    APFloat *res = apfloat_add_impl(x, y, workprec);
    if (!res) return NULL;
    if (!res) return NULL;
    if (!apfloat_round_resize(res, precision)) {
        apfloat_free(res);
        return NULL;
    }
    return res;
}

APFloat *apfloat_add_impl(APFloat *x, APFloat *y, uint32_t precision)
{
    if (x->sign != y->sign) {
        if (x->sign == 1) { // y = -1
            APFloat *pos_y = apfloat_copy_ex(y, APF_CAP(y));
            if (!pos_y) return NULL;
            pos_y->sign = 1;
            APFloat *res = apfloat_sub_impl(x, pos_y, precision);
            apfloat_free(pos_y);
            return res;
        } else { // x = -1
            APFloat *pos_x = apfloat_copy_ex(x, APF_CAP(x));
            if (!pos_x) return NULL;
            pos_x->sign = 1;
            APFloat *res = apfloat_sub_impl(y, pos_x, precision);
            apfloat_free(pos_x);
            return res;
        }
    }
    if (!apfloat_align(x, y)) return NULL;
    APInt *res_significand = apint_add_impl(x->significand, y->significand, precision);
    if (!res_significand) return NULL;
    APFloat *res = apfloat_from_apint_ex(res_significand, x->exponent, precision);
    if (!res) {
        apint_free(res_significand);
        return NULL;
    }
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
    if (!apfloat_resize(x, workprec-1)) return NULL;
    if (!apfloat_resize(y, workprec-1)) return NULL; 
    APFloat *res = apfloat_sub_impl(x, y, workprec);
    if (!res) return NULL;
    if (!apfloat_round_resize(res, ctx.precision)) {
        apfloat_free(res);
        return NULL;
    }
    return res;
}

APFloat *apfloat_sub_ex(APFloat *x, APFloat *y, uint32_t precision) {
    uint32_t workprec = precision + 1;
    if (!apfloat_resize(x, workprec-1)) return NULL;
    if (!apfloat_resize(y, workprec-1)) return NULL; 
    APFloat *res = apfloat_sub_impl(x, y, workprec);
    if (!res) return NULL;
    if (!apfloat_round_resize(res, precision)) {
        apfloat_free(res);
        return NULL;
    }
    return res;
}

APFloat *apfloat_sub_impl(APFloat *x, APFloat *y, uint32_t precision) {
    if (x->sign != y->sign) {
        // -x - +y -> -x + y
        // x - -y -> x + y
        // printf("preci %d\n", precision);
        // DEBUG_PRINT_APINT(y->significand);
        APFloat *neg_y = apfloat_copy_ex(y, APF_CAP(y));
        // DEBUG_PRINT_APINT(neg_y->significand);
        // printf("ny->sz %d, y->sz: %d", APF_SZ(neg_y), APF_SZ(y));
        // printf("ny->cp %d, y->cp: %d", APF_CAP(neg_y), APF_CAP(y));
        if (!neg_y) return NULL;
        neg_y->sign = -y->sign;
        APFloat *res = apfloat_add_impl(x, neg_y, precision);
        apfloat_free(neg_y);
        return res;
    }
    if (!apfloat_align(x, y)) return NULL;

    int cmp = apint_abs_compare(x->significand, y->significand);

    if (cmp == 0) {
        APFloat *zero = apfloat_init_ex(precision);
        APF_SZ(zero) = 1;
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
    if (!res_significand) return NULL;
    APFloat *res = apfloat_from_apint_ex(res_significand, x->exponent, precision);
    if (!res) {
        apint_free(res_significand);
        return NULL;
    }
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
    if (APF_SZ(x) > ctx.precision + 1) {
        if (!apfloat_resize(x, ctx.precision + 1)) return NULL;
    }
    if (APF_SZ(y) > ctx.precision + 1) {
        if (!apfloat_resize(y, ctx.precision + 1)) return NULL;
    }
    APFloat *res = apfloat_mul_impl(x, y, workprec);
    if (!res) return NULL;
    if (!apfloat_round_resize(res, ctx.precision)) {
        apfloat_free(res);
        return NULL;
    }
    return res;
}

APFloat *apfloat_mul_ex(APFloat *x, APFloat *y, uint32_t precision) {
    uint32_t workprec = precision * 2 + 2; 
    if (APF_SZ(x) > ctx.precision + 1) {
        if (!apfloat_resize(x, ctx.precision + 1)) return NULL;
    }
    if (APF_SZ(y) > ctx.precision + 1) {
        if (!apfloat_resize(y, ctx.precision + 1)) return NULL;
    }
    APFloat *res = apfloat_mul_impl(x, y, workprec);
    if (!res) return NULL;
    if (!apfloat_round_resize(res, precision)) {
        apfloat_free(res);
        return NULL;
    }
    return res;
}

APFloat *apfloat_mul_impl(APFloat *x, APFloat *y, uint32_t precision) {
    APInt *res_significand = apint_mul_impl(x->significand, y->significand, precision);
    if (!res_significand) return NULL;
    APFloat *res = apfloat_from_apint_ex(res_significand, x->exponent + y->exponent, precision);
    if (!res) {
        apint_free(res_significand);
        return NULL;
    }
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
    if (!apfloat_resize(x, workprec)) return NULL;
    if (!apfloat_resize(y, workprec)) return NULL;
    APFloat *res = apfloat_div_impl(x, y, workprec);
    if (!res) return NULL;
    if (!apfloat_round_resize(res, ctx.precision)) {
        apfloat_free(res);
        return NULL;
    }
    apfloat_round_resize(res, ctx.precision);
    return res;
}

APFloat *apfloat_div_ex(APFloat *x, APFloat *y, uint32_t precision) {
    uint32_t workprec = precision + 2;
    if (!apfloat_resize(x, workprec)) return NULL;
    if (!apfloat_resize(y, workprec)) return NULL;
    APFloat *res = apfloat_div_impl(x, y, workprec);
    if (!res) return NULL;
    if (!apfloat_round_resize(res, precision)) {
        apfloat_free(res);
        return NULL;
    }
    return res;
}

APFloat *apfloat_div_impl(APFloat *x, APFloat *y, uint32_t precision) {
    APInt *remainder;
    APInt *initial_significand = apint_div_impl(x->significand, y->significand, &remainder, precision);
    if (!initial_significand) return NULL;
    APFloat *res = apfloat_from_apint(initial_significand, x->exponent-y->exponent);
    if (!res) {
        apint_free(remainder);
        apint_free(initial_significand);
        return NULL;
    }
    bool should_shift = !apint_is_zero(initial_significand);
    while (!apint_is_zero(remainder) && APF_SZ(res) < precision) {
        if (remainder->size < precision) {
            if (!apint_left_shift_inplace(remainder, 1)) {
                apfloat_free(res);
                apint_free(remainder);
                return NULL;
            }
        }
        else apint_right_shift_inplace(y->significand, 1);
        DIGITS_DTYPE count = 0;
        while (apint_abs_compare(remainder, y->significand) >= 0) {
            if (!apint_sub_inplace(remainder, y->significand)) {
                apfloat_free(res);
                apint_free(remainder);
                return NULL;
            }
            count++;
        }
        if (should_shift) {
            if (!apint_left_shift_inplace(res->significand, 1)) {
                apfloat_free(res);
                apint_free(remainder);
                return NULL;
            }
        }
        if (count) should_shift = true;
        res->exponent--;
        APF_DIG(res)[0] = count;
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
    if (APF_SZ(x) > ctx.precision + y->size) {
        if (!apfloat_resize(x, ctx.precision + y->size)) return NULL;
    }

    APFloat *res = apfloat_pow_impl(x, y, workprec);
    if (!res) return NULL;
    if (!apfloat_round_resize(res, ctx.precision)) {
        apfloat_free(res);
        return NULL;
    }
    return res;
}

APFloat *apfloat_pow_ex(APFloat *x, APInt *y, uint32_t precision) {
    uint32_t workprec = precision + 1;
    if (APF_SZ(x) > precision + y->size) {
        if (!apfloat_resize(x, precision + y->size)) return NULL;
    }

    APFloat *res = apfloat_pow_impl(x, y, workprec);
    if (!res) return NULL;
    if (!apfloat_round_resize(res, precision)) {
        apfloat_free(res);
        return NULL;
    }
    return res;
}

APFloat *apfloat_pow_impl(APFloat *x, APInt *y, uint32_t precision) {
    if (y->size > 19 || (y->size == 19 && y->digits[19] >= 9)) {
        fprintf(stderr, "Maximum exponent allowed is 8,999,999,999,999,999,999");
        exit(EXIT_FAILURE);
    }
    APInt *base = apint_copy_ex(x->significand, APF_SZ(x));
    if (!base) return NULL;
    APInt *exponent = apint_copy_ex(y, y->size);
    if (!exponent) {
        apint_free(base);
        return NULL;
    }
    APInt *two = apint_init_ex(1);
    if (!two) {
        apint_free(base); apint_free(exponent);
        return NULL;
    }
    two->digits[0] = 2;
    two->size = 1;
    
    APInt *res = apint_init_ex(1);
    if (!res) {
        apint_free(base); apint_free(exponent); apint_free(two);
        return NULL;
    }
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
        if (!quotient) {
            apint_free(base); apint_free(exponent); apint_free(two); apint_free(res);
        }
        
        if (!apint_is_zero(remainder)) {
            APInt *temp = res;
            res = apint_mul_impl(res, base, res->size + base->size);
            if (!res) {
                apint_free(base); apint_free(exponent); apint_free(two); apint_free(res); apint_free(quotient); apint_free(remainder); apint_free(temp);
                return NULL;
            }

            if (res->size > precision) {
                res_exponent += (int64_t)(res->size - precision);
                if (!apint_resize(res, precision)) {
                    apint_free(base); apint_free(exponent); apint_free(two); apint_free(res); apint_free(quotient); apint_free(remainder); apint_free(temp);
                    return NULL;
                }
            }
            apint_free(temp);
        }
        apint_free(exponent);
        apint_free(remainder);
        exponent = quotient;
        
        APInt *temp = base;
        base = apint_mul_impl(base, base, base->size * 2);
        if (!base) {
            apint_free(exponent); apint_free(two); apint_free(temp);
            return NULL;
        }
        apint_free(temp);
    }
    
    apint_free(base);
    apint_free(exponent);
    apint_free(two);
    
    if (!apint_resize(res, res->size)) {
        apint_free(res);
        return NULL;
    }

    APFloat *apfloat_repr = apfloat_from_apint(res, res_exponent);
    if (!apfloat_repr) {
        apint_free(res);
        return NULL;
    }

    apfloat_normalize(apfloat_repr);
    
    if (y->sign == -1) {
        APFloat *one = apfloat_init_ex(1); 
        if (!one) {
            apfloat_free(apfloat_repr);
            return NULL;
        }
        APF_DIG(one)[0] = 1;
        APF_SZ(one) = 1;
        APFloat *res_inverse = apfloat_div_impl(one, apfloat_repr, precision);
        if (!res_inverse) {
            apfloat_free(apfloat_repr);
            apfloat_free(one);
            return NULL;
        }

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