#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "APCtx.h"
#include "APIntOps.h"
#include "APNumber.h"
#include "APError.h"

// Helpers
#if 1
int apint_abs_compare(const APInt *x, const APInt *y) {
    if (x->size > y->size) return 1;
    if (x->size < y->size) return -1;
    for (size_t i = x->size; i > 0; i--) {
        if (x->digits[i - 1] > y->digits[i - 1]) return 1;
        if (x->digits[i - 1] < y->digits[i - 1]) return -1;
    }
    return 0;
}

bool apint_left_shift_inplace(APInt *x, uint32_t n) {
    if (x->size + n > x->capacity) {
        apm_set_error(APM_ERROR_OVERFLOW, "Insufficient capacity for left shift.\n");
        return false;
    }
    memmove(x->digits + n, x->digits, x->size * sizeof(DIGITS_DTYPE));
    memset(x->digits, 0, n * sizeof(DIGITS_DTYPE));
    x->size += n;
    return true;
}

void apint_right_shift_inplace(APInt *x, uint32_t n) {
    if (n >= x->size) {
        x->size = 1;
        memset(x->digits, 0, x->capacity * sizeof(DIGITS_DTYPE));
        return;
    }
    memmove(x->digits, x->digits + n, (x->size - n) * sizeof(DIGITS_DTYPE));
    memset(x->digits + (x->size - n), 0, n * sizeof(DIGITS_DTYPE));
    x->size -= n;
}

APInt *apint_left_shift(APInt *x, uint32_t n) {
    APInt *res = apint_init_ex(x->size + n);
    if (!res) return NULL;
    memcpy(res->digits + n, x->digits, x->size * sizeof(DIGITS_DTYPE));
    res->size = x->size + n;
    return res;
}

APInt *apint_right_shift(APInt *x, uint32_t n) {
    if (n >= x->size) {
        APInt *res = apint_init_ex(x->size + n);
        if (!res) return NULL;
        res->size = 1;
        return res;
    }
    APInt *res = apint_init_ex(x->size + n);
    if (!res) return NULL;
    memcpy(res->digits, x->digits + n, (x->size - n) * sizeof(DIGITS_DTYPE));
    res->size = res->size - n;
    return res;
}
#endif

// Addition
#if 1

APInt *apint_add(APInt *x, APInt *y) {
    return apint_add_impl(x, y, x->size >= y->size ? x->size+1 : y->size+1);
}

bool apint_add_inplace(APInt *x, APInt *y) {
    if (x->sign != y->sign) {
        if (x->sign == 1) {
            APInt *pos_y = apint_copy_ex(y, y->capacity);
            if (!pos_y) return false;
            pos_y->sign = 1;
            if (!apint_sub_inplace(x, pos_y)) {
                apint_free(pos_y);
                return false;
            }
            apint_free(pos_y);
            return true;
        } else {
            APInt *pos_x = apint_copy_ex(x, x->capacity);
            if (!pos_x) return false;
            pos_x->sign = 1;
            if (!apint_sub_inplace(y, pos_x)) { apint_free(pos_x); return false; }
            if (!apint_copy_into_resize(x, y)) { apint_free(pos_x); return false; }
            apint_free(pos_x);
            return true;
        }
    }
    uint32_t max_size = x->size > y->size ? x->size + 1 : y->size + 1;
    DIGITS_DTYPE carry = 0;
    for (uint32_t i = 0; (i < x->size || i < y->size || carry); i++) {
        DIGITS_DTYPE sum = carry;
        if (i < x->size) sum += x->digits[i];
        if (i < y->size) sum += y->digits[i];
        if (i >= x->capacity) if (!apint_resize(x, x->capacity+1)) return false;
        x->digits[i] = sum % 10;
        carry = sum / 10;
    }
    x->size = max_size;
    apint_normalize(x);
    return true;
}

APInt *apint_add_impl(APInt *x, APInt *y, uint32_t precision) {
    if (x->sign != y->sign) {
        if (x->sign == 1) { // y = -1
            APInt *pos_y = apint_copy_ex(y, y->capacity);
            if (!pos_y) return NULL;
            pos_y->sign = 1;
            APInt *res = apint_sub_impl(x, pos_y, precision);
            apint_free(pos_y);
            return res;
        } else { // x = -1
            APInt *pos_x = apint_copy_ex(x, x->capacity);
            if (!pos_x) return NULL;
            pos_x->sign = 1;
            APInt *res = apint_sub_impl(y, pos_x, precision);
            apint_free(pos_x);
            return res;
        }
    }
    APInt *res = apint_init_ex(precision);
    if (!res) return NULL;
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
    return apint_sub_impl(x, y, x->size >= y->size ? x->size+1 : y->size+1);
}

bool apint_sub_inplace(APInt *x, APInt *y) {
    if (x->sign != y->sign) {
        APInt *neg_y = apint_copy_ex(y, y->capacity);
        if (!neg_y) return false;
        neg_y->sign = -y->sign;
        if (!apint_add_inplace(x, neg_y)) { apint_free(neg_y); return false; }
        apint_free(neg_y);
        return true;
    }
    
    int cmp = apint_abs_compare(x, y);
    if (cmp == 0) {
        x->size = 1;
        x->digits[0] = 0;
        x->sign = 1;
        return true;
    }
    
    const APInt *larger = x, *smaller = y;
    int result_sign = x->sign;
    if (cmp < 0) {
        larger = y;
        smaller = x;
        result_sign = -result_sign;
    }
    
    int borrow = 0;
    for (size_t i = 0; i < larger->size; i++) {
        int diff = larger->digits[i] - borrow;
        if (i < smaller->size) diff -= smaller->digits[i];
        
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else borrow = 0;
        
        assert(diff >= 0 && diff < 10);
        x->digits[i] = diff;
    }
    x->size = larger->size;
    x->sign = result_sign;
    apint_normalize(x);
    return true;
}

APInt *apint_sub_impl(APInt *x, APInt *y, uint32_t precision) {
    // If x and y have different signs, negate y and perfom an add
    if (x->sign != y->sign) {
        APInt *neg_y = apint_copy_ex(y, y->capacity);
        if (!neg_y) return NULL;
        neg_y->sign = -y->sign;
        APInt *res = apint_add_impl(x, neg_y, precision);
        apint_free(neg_y);
        return res;
    }
    // Compare x and y
    int cmp = apint_abs_compare(x, y);
    // If equal return 0
    if (cmp == 0) {
        APInt *zero = apint_init();
        if (!zero) return NULL;
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
    if (!res) return NULL;
    res->sign = result_sign;
    
    int borrow = 0;
    for (size_t i = 0; i < larger->size; i++) {
        int diff = larger->digits[i] - borrow;
        if (i < smaller->size) diff -= smaller->digits[i];

        // If diff is negative, borrow 10 from the next digit (note that we always know this will work since we always do larger - smaller)
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else borrow = 0;
        assert(diff >= 0 && diff < 10);
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
    return apint_mul_impl(x, y, x->size+y->size);
}

APInt *apint_mul_impl(APInt *x, APInt *y, uint32_t precision) {
    if (apint_is_zero(x) || apint_is_zero(y)) {
        APInt *zero = apint_init_ex(precision);
        if (!zero) return NULL;
        zero->size = 1;
        return zero;
    }
    // loop through x and y
    APInt *res = apint_init_ex(precision);
    if (!res) return NULL;
    for (uint32_t i = 0; i < x->size; i++) {
        DIGITS_DTYPE carry = 0;
        // While we have values for y[j] or we have a carry, continue
        for (uint32_t j = 0; j < y->size || carry; j++) {
            DIGITS_DTYPE product = res->digits[i + j] + x->digits[i] * (j < y->size ? y->digits[j] : 0) + carry;
            assert(res->digits[i + j] < 10);
            assert(x->digits[i] < 10);
            assert(y->digits[j] < 10);
            assert(carry < 10);
            assert(product < 100 && product >= 0);
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
    return apint_div_impl(x, y, remainder, x->size >= y->size ? x->size : y->size);
}

APInt *apint_div_impl(APInt *x, APInt *y, APInt **remainder, uint32_t precicion) {
    if (apint_is_zero(y)) {
        apm_set_error(APM_ERROR_DIVISION_BY_ZERO, "Division by zero.\n");
        return NULL;
    }

    APInt *res = NULL;
    APInt *rem = NULL;
    APInt *shifted_y = NULL;

    if (apint_abs_compare(x, y) == -1) {
        APInt *res = apint_init_ex(precicion);
        if (!res) return NULL;
        res->size = 1;
        res->digits[0] = 0;
        if (remainder) {
            *remainder = apint_copy_ex(x, precicion);
            if (!*remainder) goto error_cleanup;
        }
        return res;
    }

    int result_sign = 1;

    rem = apint_copy_ex(x, precicion);
    if (!rem) goto error_cleanup;
    int shift = (int)(x->size - y->size);
    shifted_y = apint_left_shift(y, shift);
    if (!shifted_y) goto error_cleanup;
    res = apint_init_ex(precicion);
    if (!res) goto error_cleanup;

    if (x->sign != y->sign) result_sign = -1;
    rem->sign = 1;
    shifted_y->sign = 1;

    for (int i = shift; i >= 0; i--) {
        int count = 0;
        while (apint_abs_compare(rem, shifted_y) >= 0) { // while dividend >= divisor 
            if (!apint_sub_inplace(rem, shifted_y)) goto error_cleanup;
            // apint_free(rem);
            // rem = new_rem;
            count++;
        }
        assert(count < 10);
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
error_cleanup:
    apint_free(res);
    apint_free(rem);
    apint_free(shifted_y);
    if (remainder) *remainder = NULL;
    return NULL;
}

#endif

// Exponentiation
# if 1

APInt *apint_pow(APInt *x, APInt *y) {
    if (y->sign == -1) {
        fprintf(stderr, "Error: Integer exponentiation only works on exponents greater than or equal to 0.\n");
        exit(1);
    }

    APInt *base = NULL, *exponent = NULL, *two = NULL, *res = NULL;
    APInt *quotient = NULL, *remainder = NULL, *temp = NULL;

    base = apint_copy_ex(x, x->size);
    if (!base) goto error_cleanup;
    exponent = apint_copy_ex(y, y->size);
    if (!exponent) goto error_cleanup;
    two = apint_init_ex(1);
    if (!two) goto error_cleanup;
    two->digits[0] = 2;
    two->size = 1;

    res = apint_init_ex(1);
    if (!res) goto error_cleanup;

    res->digits[0] = 1;
    res->size = 1;

    while (!apint_is_zero(exponent)) {
        quotient = apint_div_impl(exponent, two, &remainder, exponent->size);
        if (!quotient || !remainder) goto error_cleanup;

        if (!apint_is_zero(remainder)) {
            temp = res;
            res = apint_mul_impl(res, base, res->size + base->size);
            if (!res) goto error_cleanup;
            apint_free(temp);
            temp = NULL;
        }
        apint_free(exponent);
        apint_free(remainder);
        exponent = quotient;
        remainder = NULL;
        quotient = NULL;
        
        temp = base;
        base = apint_mul_impl(base, base, base->size * 2);
        if (!base) goto error_cleanup;
        apint_free(temp);
        temp = NULL;
    }
    
    if (!apint_resize(res, res->size)) goto error_cleanup;

    apint_free(base);
    apint_free(exponent);
    apint_free(two);

    apint_normalize(res);
    return res;
error_cleanup:
    apint_free(base);
    apint_free(exponent);
    apint_free(two);
    apint_free(res);
    apint_free(quotient);
    apint_free(remainder);
    apint_free(temp);
    return NULL;
}

#endif
