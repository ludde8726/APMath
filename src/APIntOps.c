#include "APCtx.h"
#include "APIntOps.h"
#include "APNumber.h"

int apint_abs_compare(const APInt *x, const APInt *y) {
    // compare x and y
    // x > y -> 1
    // equal -> 0
    // y < x -> -1
    if (x->size > y->size) return 1;
    if (x->size < y->size) return -1;
    for (size_t i = x->size; i > 0; i--) {
        if (x->digits[i - 1] > y->digits[i - 1]) return 1;
        if (x->digits[i - 1] < y->digits[i - 1]) return -1;
    }
    return 0;
}

// Addition
#if 1

APInt *apint_add(APInt *x, APInt *y) {
    // Set working precision to 2 more then precision
    uint32_t workprec = ctx.precision + 2;

    // Resize operands, example:
    // 123456 + 123456 calculated with global precision of 3 will resize to
    // 1235 + 1235 = 2470, then resize to correct prec -> 247
    // The same result would happen from calculating first and then resizing
    // 123456 + 123456 = 246912, then resize to correct prec -> 247
    // The reason for resizing early is to avoid unnecessary calculations
    if (x->size > workprec-1) apint_resize(x, workprec-1);
    if (y->size > workprec-1) apint_resize(y, workprec-1);
    APInt *res = apint_add_ex(x, y, workprec);
    apint_resize(res, ctx.precision);
    return res;
}

APInt *apint_add_ex(APInt *x, APInt *y, uint32_t precision) {
    // This function aussumes the precision is big enough to store the result, if not it will cause a segmentation fault.
    if (x->sign != y->sign) {
        if (x->sign == 1) { // y = -1
            APInt *pos_y = apint_copy_ex(y, y->capacity);
            pos_y->sign = 1;
            APInt *res = apint_sub_ex(x, pos_y, precision);
            apint_free(pos_y);
            return res;
        } else { // x = -1
            APInt *pos_x = apint_copy_ex(x, x->capacity);
            pos_x->sign = 1;
            APInt *res = apint_sub_ex(y, pos_x, precision);
            apint_free(pos_x);
            return res;
        }
    }
    APInt *res = apint_init_ex(precision);
    res->sign = x->sign;
    
    DIGITS_DTYPE carry = 0;
    for (uint32_t i = 0; i < precision && (i < x->size || i < y->size || carry); i++) { // Don't do unneccesary iteraions
        DIGITS_DTYPE sum = carry;
        if (i < x->size) sum += x->digits[i];
        if (i < y->size) sum += y->digits[i];
        res->digits[i] = sum % 10;
        carry = sum / 10;
    }
    res->size = precision;
    apint_normalize(res);
    return res;
}

# endif

// Subtraction
#if 1

APInt *apint_sub(APInt *x, APInt *y) {
    uint32_t workprec = ctx.precision + 2;
    if (x->size > workprec-1) apint_resize(x, workprec-1);
    if (y->size > workprec-1) apint_resize(y, workprec-1);
    APInt *res = apint_sub_ex(x, y, workprec);
    apint_resize(res, ctx.precision);
    return res;
}

APInt *apint_sub_ex(APInt *x, APInt *y, uint32_t precision) {
    // If x and y have different signs, negate y and perfom an add
    if (x->sign != y->sign) {
        APInt *neg_y = apint_copy(y);
        neg_y->sign = -1;
        APInt *res = apint_add_ex(x, neg_y, precision);
        apint_free(neg_y);
        return res;
    }
    // Compare x and y
    int cmp = apint_abs_compare(x, y);
    // If equal return 0
    if (cmp == 0) {
        APInt *zero = apint_init();
        zero->size = 1;
        zero->digits[0] = 0;
        zero->sign = 1;
        return zero;
    }
    
    // By default set x larger than y
    const APInt *larger = x, *smaller = y;
    int result_sign = x->sign;
    // If y is bigger than x change larger and smaller and negate the result sign
    if (cmp < 0) {
        larger = y;
        smaller = x;
        result_sign = -result_sign;
    }

    APInt *res = apint_init_ex(precision);
    res->sign = result_sign;
    
    int borrow = 0;
    for (size_t i = 0; i < larger->size; i++) {
        int diff = larger->digits[i] - borrow;
        if (i < smaller->size) diff -= smaller->digits[i];

        // If diff it negative, borrow 10 from the next digit (note that we always know this will work since we always do larger - smaller)
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else borrow = 0;
        res->digits[i] = diff;
    }
    res->size = larger->size;
    apint_normalize(res);
    return res;
}

#endif

APInt *apint_mul(APInt *x, APInt *y);
APInt *apint_mul_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_div(APInt *x, APInt *y);
APInt *apint_div_ex(APInt *x, APInt *y, uint32_t precision);
APInt *apint_pow(APInt *x, APInt *y);
APInt *apint_pow_ex(APInt *x, APInt *y, uint32_t precision);