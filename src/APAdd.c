#include "APMath.h"
#include "APAdd.h"

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

void apint_shift_right(APInt *num, size_t shift) {
    if (shift >= num->size) {
        num->size = 1;
        num->digits[0] = 0;
        return;
    }
    memmove(num->digits, num->digits + shift, (num->size - shift) * sizeof(int));
    num->size -= shift;
}

void apfloat_align_exponents(APFloat *x, APFloat *y) {
    if (x->exponent > y->exponent) {
        apint_shift_right(y->significand, x->exponent - y->exponent);
        y->exponent = x->exponent;
    } else if (y->exponent > x->exponent) {
        apint_shift_right(x->significand, y->exponent - x->exponent);
        x->exponent = y->exponent;
    }
}

APInt *apint_add(APInt* x, APInt *y) {
    if (x->sign != y->sign) {
        if (x->sign == 1) {
            // x + -y -> x - y
            APInt *pos_y = init_apint(y->capacity);
            memcpy(pos_y->digits, y->digits, y->size*sizeof(int));
            pos_y->size = y->size;
            APInt *res = apint_sub(x, pos_y);
            free_apint(pos_y);
            return res;
        } else {
            // -x + y -> x - y
            APInt *pos_x = init_apint(x->capacity);
            memcpy(pos_x->digits, x->digits, x->size*sizeof(int));
            pos_x->size = x->size;
            APInt *res = apint_sub(y, pos_x);
            free_apint(pos_x);
            return res;
        }
    }

    size_t max_size = (x->size > y->size ? x->size : y->size) + 1;
    APInt *res = init_apint(max_size);
    res->sign = x->sign;
    
    int carry = 0;
    for (size_t i = 0; i < max_size; i++) {
        int sum = carry;
        if (i < x->size) sum += x->digits[i];
        if (i < y->size) sum += y->digits[i];
        res->digits[i] = sum % 10;
        carry = sum / 10;
    }
    res->size = max_size;
    apint_normalize(res);
    return res;
}

APFloat *apfloat_add(APFloat *x, APFloat *y) {
    APFloat *aligned_x = copy_apfloat(x);
    APFloat *aligned_y = copy_apfloat(y);
    size_t max_capacity = (x->significand->capacity > y->significand->capacity ? x->significand->capacity : y->significand->capacity);
    APFloat *res = init_apfloat(max_capacity);
    apfloat_align_exponents(aligned_x, aligned_y);

    if (aligned_x->sign == aligned_y->sign) {
        res->significand = apint_add(aligned_x->significand, aligned_y->significand);
        res->sign = aligned_x->sign;
    } else {
        if (aligned_x->sign == 1) {
            // x + -y
            aligned_y->sign = 1;
            res->significand = apint_sub(aligned_x->significand, aligned_y->significand);
            res->sign = res->significand->sign;
            res->significand->sign = 1;
        } else {
            // -x + y
            aligned_x->sign = 1;
            res->significand = apint_sub(aligned_y->significand, aligned_x->significand);
            res->sign = res->significand->sign;
            res->significand->sign = 1;
        }
    }
    res->exponent = aligned_x->exponent;
    free_apfloat(aligned_x);
    free_apfloat(aligned_y);
    apfloat_normalize(res);
    return res;
}

APComplex *apcomplex_add(APComplex *x, APComplex *y) {
    APComplex *res = init_apcomplex(x->real->significand->capacity + 1); // NOTE that this will later change into the globally set precision.
    res->real = apfloat_add(x->real, y->real);
    res->imag = apfloat_add(x->imag, y->imag);
    return res;
}

APInt *apint_sub(APInt *x, APInt *y)
{
    if (x->sign != y->sign) {
        // -x - y or x - -y -> x + y or -x + y
        APInt *neg_y = init_apint(y->capacity);
        memcpy(neg_y->digits, y->digits, y->size*sizeof(int));
        neg_y->size = y->size;
        neg_y->sign = -y->sign;
        APInt *res = apint_add(x, neg_y);
        free_apint(neg_y);
        return res;
    }
    
    int cmp = apint_abs_compare(x, y);
    if (cmp == 0) {
        APInt *zero = init_apint(1);
        zero->size = 1;
        zero->digits[0] = 0;
        zero->sign = 1;
        return zero;
    }

    const APInt *larger = x, *smaller = y;
    int result_sign = x->sign;
    if (cmp < 0) {
        larger = y;
        smaller = x;
        result_sign = -result_sign;
    }

    APInt *res = init_apint(larger->size); // The max size will always be the same size as the largest argument
    res->sign = result_sign;
    
    int borrow = 0;
    for (size_t i = 0; i < larger->size; i++) {
        int diff = larger->digits[i] - borrow;
        if (i < smaller->size) diff -= smaller->digits[i];

        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        res->digits[i] = diff;
        res->size = larger->size;
    }
    res->size = larger->size;
    apint_normalize(res);
    return res;
}

APFloat *apfloat_sub(APFloat *x, APFloat *y)
{
    APFloat *neg_y = copy_apfloat(y);
    neg_y->sign = -y->sign;
    APFloat *res = apfloat_add(x, neg_y);
    free_apfloat(neg_y); // It might be faster to just change the sign of y and then change back since that means no copy.
    return res;
}

APComplex *apcomplex_sub(APComplex *x, APComplex *y)
{
    APComplex *res = init_apcomplex(x->real->significand->capacity); // NOTE that this will later change into the globally set precision.
    res->real = apfloat_sub(x->real, y->real);
    res->imag = apfloat_sub(x->imag, y->imag);
    return res;
}
