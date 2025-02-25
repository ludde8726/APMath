#include <string.h>
#include <stdlib.h>

#include "APCtx.h"
#include "APIntOps.h"
#include "APNumber.h"

// Helpers, compare, shift, etc.
#if 1
// Compare the absolute values of x and y
// if x > y -> 1, if x < y -> -1, if x == y -> 0
int apint_abs_compare(const APInt *x, const APInt *y) {
    if (x->size > y->size) return 1;
    if (x->size < y->size) return -1;
    for (size_t i = x->size; i > 0; i--) {
        if (x->digits[i - 1] > y->digits[i - 1]) return 1;
        if (x->digits[i - 1] < y->digits[i - 1]) return -1;
    }
    return 0;
}

// In-place left shift: Multiply the number by 10^n.
// This is achieved by shifting the digits right in the digits array (toward higher indices).
// Note that this function modifies x.
void apint_left_shift_inplace(APInt *x, uint32_t n) {
    if (x->size + n > x->capacity) {
        fprintf(stderr, "Insufficient capacity for left shift.\n");
        exit(1);
    }
    memmove(x->digits + n, x->digits, x->size * sizeof(DIGITS_DTYPE));
    memset(x->digits, 0, n * sizeof(DIGITS_DTYPE));
    x->size += n;
}

// In-place left shift: Divide the number by 10^n.
// This is achieved by shifting the digits left in the digits array (toward lower indices).
// Note that this function modifies x.
void apint_right_shift_inplace(APInt *x, uint32_t n) {
    if (n >= x->size) {
        x->size = 1;
        memset(x->digits, 0, x->capacity * sizeof(DIGITS_DTYPE));
        return;
    }
    memmove(x->digits, x->digits + n, (x->size - n) * sizeof(DIGITS_DTYPE));
    x->size -= n;
}

// Left shift: Multiply the number by 10^n
// This is achieved by shifting the digits right in the digits array (toward higher indices).
// Note that this function returns a new shifted APInt.
APInt *apint_left_shift(APInt *x, uint32_t n) {
    APInt *res = apint_init_ex(x->size + n);
    memcpy(res->digits + n, x->digits, x->size * sizeof(DIGITS_DTYPE));
    res->size = x->size + n;
    return res;
}

// Right shift: Divide the number by 10^n
// This is achieved by shifting the digits left in the digits array (toward lower indices).
// Note that this function returns a new shifted APInt.
APInt *apint_right_shift(APInt *x, uint32_t n) {
    if (n >= x->size) {
        APInt *res = apint_init_ex(x->size + n);
        res->size = 1;
        return res;
    }
    APInt *res = apint_init_ex(x->size + n);
    memcpy(res->digits, x->digits + n, (x->size - n) * sizeof(DIGITS_DTYPE));
    res->size = res->size - n;
    return res;
}
#endif

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
        neg_y->sign = -y->sign;
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

// Multiplication
#if 1

APInt *apint_mul(APInt *x, APInt *y) {
    // For multiplication n + 1 significant digits is needed for the operands to achive correct result for n digits of precision
    uint32_t workprec = ctx.precision * 2 + 2; 
    if (x->size > ctx.precision + 1) apint_resize(x, ctx.precision + 1);
    if (y->size > ctx.precision + 1) apint_resize(y, ctx.precision + 1);
    APInt *res = apint_mul_ex(x, y, workprec);
    apint_resize(res, ctx.precision);
    return res;
}

APInt *apint_mul_ex(APInt *x, APInt *y, uint32_t precision) {
    if (apint_is_zero(x) || apint_is_zero(y)) {
        APInt *zero = apint_init_ex(precision);
        zero->size = 1;
        zero->digits[0] = 0;
        return zero;
    }
    // loop through x and y
    APInt *res = apint_init_ex(precision);
    for (uint32_t i = 0; i < x->size; i++) {
        DIGITS_DTYPE carry = 0;
        // While we have values for y[j] or we have a carry, continue
        for (uint32_t j = 0; j < y->size || carry; j++) {
            DIGITS_DTYPE product = res->digits[i + j] + x->digits[i] * (j < y->size ? y->digits[j] : 0) + carry;
            res->digits[i + j] = product % 10;
            carry = product / 10;
        }
    }
    res->size = x->size + y->size;
    res->sign = x->sign == y->sign ? 1 : -1;
    apint_normalize(res);
    return res;
}

#endif

// Division

#if 1

APInt *apint_div(APInt *x, APInt *y, APInt **remainder) {
    uint32_t workprec = ctx.precision + 1;
    // For division to be correct we need at least n+1 (significant) digits while also keeping the magnitude
    if (x->size > workprec) {
        uint32_t x_diff = x->size - workprec;
        apint_right_shift_inplace(x, x_diff);
        apint_left_shift_inplace(x, x_diff);
    }
    if (y->size > workprec) {
        uint32_t y_diff = y->size - workprec;
        apint_right_shift_inplace(y, y_diff);
        apint_left_shift_inplace(y, y_diff);
    }
    // We have to perform the calculation on the biggest possible size of x or y.
    // However we still normalize them as small as possible not perform any unnecessary calculations.
    workprec = x->size >= y->size ? x->size : y->size;
    APInt *res = apint_div_ex(x, y, remainder, workprec);
    apint_resize(res, ctx.precision);
    return res;
}

APInt *apint_div_ex(APInt *x, APInt *y, APInt **remainder, uint32_t precicion) { // https://chatgpt.com/c/67be0105-3354-800a-8a00-e1c16cb16226
    if (apint_is_zero(y)) {
        fprintf(stderr, "Error: Division by zero.\n");
        exit(1);
    }

    if (apint_abs_compare(x, y) == -1) {
        APInt *res = apint_init_ex(precicion);
        res->size = 1;
        res->digits[0] = 0;
        if (remainder) *remainder = apint_copy_ex(x, precicion);
        return res;
    }

    int result_sign = 1;

    APInt *rem = apint_copy_ex(x, precicion);
    int shift = (int)(x->size - y->size);
    APInt *shifted_y = apint_left_shift(y, shift);
    APInt *res = apint_init_ex(precicion);

    if (x->sign != y->sign) result_sign = -1;
    rem->sign = 1;
    shifted_y->sign = 1;

    for (int i = shift; i >= 0; i--) {
        int count = 0;
        while (apint_abs_compare(rem, shifted_y) >= 0) { // while dividend >= divisor 
            APInt *new_rem = apint_sub_ex(rem, shifted_y, precicion);
            // for (uint32_t i = 0; i < rem->capacity; i++) printf("%d", rem->digits[i]);
            // printf(" - ");
            // for (uint32_t i = 0; i < shifted_y->capacity; i++) printf("%d", shifted_y->digits[i]);
            // printf(" = ");
            // for (uint32_t i = 0; i < new_rem->capacity; i++) printf("%d", new_rem->digits[i]);
            // printf("\n");
            apint_free(rem);
            rem = new_rem;
            count++;
        }
        res->digits[i] = count;
        res->size++;
        apint_right_shift_inplace(shifted_y, 1);
    }
    apint_normalize(res);
    apint_normalize(rem);
    res->sign = result_sign;
    rem->sign = 1;
    if (remainder) *remainder = rem;
    else apint_free(rem);
    apint_free(shifted_y);
    return res;
}
#endif

APInt *apint_pow(APInt *x, APInt *y);
APInt *apint_pow_ex(APInt *x, APInt *y, uint32_t precision);