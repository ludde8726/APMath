#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "APCtx.h"
#include "APNumber.h"
#include "APIntOps.h"
#include "APError.h"

// APInt helper functions
#if 1

APInt *apint_init(void) {
    // Create APInt and allocate memory for digits pointer
    APInt *x = (APInt*)malloc(sizeof(APInt));
    if (!x) REPORT_ERROR_OOM(NULL);

    *x = (APInt) {
        .sign = 1,
        .size = 0,
        .capacity = ctx.precision,
        .digits = calloc(ctx.precision, sizeof(DIGITS_DTYPE))
    };

    if (!x->digits) {
        free(x);
        REPORT_ERROR_OOM(NULL);
    }

    return x;
}

APInt *apint_init_ex(uint32_t precision) {
    // Create APInt and allocate memory for digits pointer
    APInt *x = (APInt*)malloc(sizeof(APInt));
    if (!x) REPORT_ERROR_OOM(NULL);

    *x = (APInt) {
        .sign = 1,
        .size = 0,
        .capacity = precision,
        .digits = calloc(precision, sizeof(DIGITS_DTYPE))
    };

    if (!x->digits) {
        free(x);
        REPORT_ERROR_OOM(NULL);
    }

    return x;
}

APInt *apint_copy(APInt *x) {
    // A function for creating a copy of apint, note that the allocated space will depend on the currently set precision, this may result in loss of data.
    APInt *res = apint_init();
    if (!res) return NULL;
    res->sign = x->sign;
    // If the size of x i greater than the currect precision, loose the least significant digits.
    if (x->size > res->capacity) memcpy(res->digits, x->digits + (x->size - res->capacity), res->capacity * sizeof(DIGITS_DTYPE));
    // Else copy digits directly from x to res
    else memcpy(res->digits, x->digits, x->size * sizeof(DIGITS_DTYPE));
    res->size = (x->size > res->capacity) ? res->capacity : x->size;
    return res;
}

APInt *apint_copy_ex(APInt *x, uint32_t precision) {
    APInt *res = apint_init_ex(precision);
    if (!res) return NULL;
    res->sign = x->sign;
    if (x->size > precision) memcpy(res->digits, x->digits + (x->size - precision), precision * sizeof(DIGITS_DTYPE));
    else memcpy(res->digits, x->digits, x->size * sizeof(DIGITS_DTYPE));
    res->size = (x->size > precision) ? precision : x->size;
    return res;
}

void apint_copy_into(APInt *x, APInt *y) {
    if (x->capacity < y->size) memcpy(x->digits, y->digits + (y->size - x->capacity), x->capacity * sizeof(DIGITS_DTYPE));
    else memcpy(x->digits, y->digits, y->size * sizeof(DIGITS_DTYPE));
    x->size = y->size;
    x->sign = y->sign;
}

bool apint_copy_into_resize(APInt *x, APInt *y) {
    if (!apint_resize(x, y->capacity)) return false;
    apint_copy_into(x, y);
    return true;
}

bool apint_round_at_n(APInt *x, uint32_t n) {
    if (n - 1 > x->size) {
        apm_set_error(ROUNDING_ERROR, "Cannot round at non existant digit.");
        return false;
    }

    if (x->digits[x->size - n - 1] >= 5) {
        uint32_t carry = 1;
        for (uint32_t i = x->size - n; i < x->size; i++) {
            uint32_t sum = x->digits[i] + carry;
            x->digits[i] = sum % 10;
            carry = sum / 10;
            if (!carry) break;
        }
        if (carry) {
            memset(x->digits + x->size - n, 0, n * sizeof(DIGITS_DTYPE));
            x->digits[x->size-1] = 1;
        }
    }
    return true;
}

bool apint_resize(APInt *x, uint32_t precision) {
    DIGITS_DTYPE *new_digits = calloc(precision, sizeof(DIGITS_DTYPE));
    if (!new_digits) REPORT_ERROR_OOM(false); // Memory allocation failed.
    // If the size of x is greater than the precision, loose the least significant digits.
    if (x->size > precision) {
        memmove(new_digits, x->digits + (x->size - precision), precision * sizeof(DIGITS_DTYPE));
    // Else copy digits directly from digits to new_digits
    } else memmove(new_digits, x->digits, x->size * sizeof(DIGITS_DTYPE));
    // Freeing the old digits
    free(x->digits);
    // Assigning the new
    x->digits = new_digits;
    x->size = precision;
    x->capacity = precision;
    apint_normalize(x);
    return 1;
}

bool apint_round_resize(APInt *x, uint32_t precision) {
    if (!apint_round_at_n(x, precision)) return false;
    return apint_resize(x, precision);
}

APInt *apint_from_int(long long x) {
    // Create an apint from a regular int
    APInt *res = apint_init();
    if (!res) return NULL;

    res->sign = x >= 0 ? 1 : -1;
    uint32_t intlen = x == 0 ? 1 : (uint32_t)log10(llabs(x)) + 1;
    if (x < 0) x = llabs(x);
    // If the length of the number is greater than the current precision, truncate the least significant digits.
    if (intlen > res->capacity) x /= (int)pow(10, intlen-res->capacity);
    int idx = 0;
    do {
        res->digits[idx] = x % 10;
        x /= 10;
        idx++;
        res->size++;
    } while (x);
    return res;
}

void apint_free(APInt *x) {
    if (x->digits) free(x->digits);
    free(x);
}

bool apint_is_zero(APInt *x) {
    return x->size == 0 || (x->size == 1 && x->digits[0] == 0);
}

void apint_normalize(APInt *x) {
    // Fix any issues with the representation
    //  - If zero, make sure size is 1
    //  - Remove leading zeroes from size
    assert(x->size <= x->capacity);
    // for (uint32_t i = x->size; i > 0; i--) assert(x->digits[i-1] < 10);
    while (x->size > 1 && x->digits[x->size - 1] == 0) x->size--;
    if (x->size == 0) {
        x->size = 1;
        x->digits[0] = 1;
    }
}

#endif

// APFloat helper functions
#if 1

APFloat *apfloat_init(void) {
    APFloat *x = (APFloat*)malloc(sizeof(APFloat));
    if (!x) REPORT_ERROR_OOM(NULL);

    *x = (APFloat) {
        .sign = 1,
        .significand = apint_init(),
        .exponent = 0
    };

    if (!x->significand) {
        free(x);
        return NULL;
    }

    return x;
}

APFloat *apfloat_init_ex(uint32_t precision) {
    APFloat *x = (APFloat*)malloc(sizeof(APFloat));
    if (!x) REPORT_ERROR_OOM(NULL);

    *x = (APFloat) {
        .sign = 1,
        .significand = apint_init_ex(precision),
        .exponent = 0
    };

    if (!x->significand) {
        free(x);
        return NULL;
    }

    return x;
}

APFloat *apfloat_copy(APFloat *x) {
    APFloat *res = apfloat_init();
    if (!res) return NULL;

    *res = (APFloat) {
        .sign = x->sign,
        .significand = apint_copy(x->significand),
        .exponent = x->exponent
    };

    if (!res->significand) {
        free(res);
        return NULL;
    }

    return res;
}

APFloat *apfloat_copy_ex(APFloat *x, uint32_t precision) {
    APFloat *res = apfloat_init();
    if (!res) return NULL;

    *res = (APFloat) {
        .sign = x->sign,
        .significand = apint_copy_ex(x->significand, precision),
        .exponent = x->exponent
    };
    
    if (!res->significand) {
        free(res);
        return NULL;
    }

    return res;
}

bool apfloat_round(APFloat *x, uint32_t n) {
    if (n > APF_SZ(x)) {
        apm_set_error(ROUNDING_ERROR, "Cannot round float to more digits than its current size.");
        return false;
    }
    if (APF_DIG(x)[APF_SZ(x) - n - 1] >= 5) {
        uint32_t carry = 1;
        for (uint32_t i = APF_SZ(x) - n; i < APF_SZ(x); i++) {
            uint32_t sum = APF_DIG(x)[i] + carry;
            APF_DIG(x)[i] = sum % 10;
            carry = sum / 10;
            if (!carry) break;
        }
        if (carry) {
            memset(APF_DIG(x) + APF_SZ(x) - n, 0, n * sizeof(DIGITS_DTYPE));
            APF_DIG(x)[APF_SZ(x)-1] = 1;
            x->exponent++;
        }
    }
    return true;
}

bool apfloat_resize(APFloat *x, uint32_t precision) {
    if (APF_SZ(x) > precision) x->exponent += APF_SZ(x) - precision;
    return apint_resize(x->significand, precision);
}

bool apfloat_round_resize(APFloat *x, uint32_t precision) {
    if (APF_SZ(x) > precision) {
        if (!apfloat_round(x, precision)) return false;
    }
    return apfloat_resize(x, precision);
}

APFloat *apfloat_from_apint(APInt *x, int64_t exponent) {
    APFloat *res = (APFloat*)malloc(sizeof(APFloat));
    if (!res) REPORT_ERROR_OOM(NULL);

    *res = (APFloat) {
        .sign = x->sign,
        .significand = x,
        .exponent = exponent
    };

    x->sign = 1;
    return res;
}

APFloat *apfloat_from_apint_ex(APInt *x, int64_t exponent, uint32_t precision) {
    APFloat *res = (APFloat*)malloc(sizeof(APFloat));
    if (!res) REPORT_ERROR_OOM(NULL);

    *res = (APFloat) {
        .sign = x->sign,
        .significand = x,
        .exponent = exponent
    };

    x->sign = 1;
    apfloat_resize(res, precision);
    return res;
}

void apfloat_free(APFloat *x) {
    if (x->significand) apint_free(x->significand);
    free(x);
}

void apfloat_normalize(APFloat *x) {
    apint_normalize(x->significand);
    x->significand->sign = 1;
    if (!apint_is_zero(x->significand)) {
        while (APF_DIG(x)[0] == 0) {
            apint_right_shift_inplace(x->significand, 1);
            x->exponent++;
        }
    } else x->sign = 1;
}

bool apfloat_is_zero(APFloat *x) {
    return apint_is_zero(x->significand);
}

bool apfloat_is_int(APFloat *x) {
    // 0001 -3 = 000.1 = 1.000
    // 000007 -2 = 00.0007 = 7000.00 
    if (x->exponent >= 0) return true;
    for (uint64_t i = -x->exponent; i > 0; i--)
        if (APF_DIG(x)[i-1] != 0) return false;
    return true;
}

#endif

// APComplex helper functions
#if 1
#endif