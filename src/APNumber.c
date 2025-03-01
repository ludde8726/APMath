#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "APCtx.h"
#include "APNumber.h"
#include "APIntOps.h"

// APInt helper functions
#if 1

APInt *apint_init() {
    // Create APInt and allocate memory for digits pointer
    APInt *num = (APInt*)malloc(sizeof(APInt));
    if (!num) return NULL;
    num->sign = 1;
    num->size = 0;
    // Since APInt is only used as the significand of APFloat the capacity never has to be bigger than the currently set precision.
    num->capacity = ctx.precision; 
    num->digits = calloc(num->capacity, sizeof(DIGITS_DTYPE));
    if (!num->digits) {
        free(num);
        return NULL;
    }
    return num;
}

APInt *apint_init_ex(uint32_t precision) {
    // Create APInt and allocate memory for digits pointer
    APInt *num = (APInt*)malloc(sizeof(APInt));
    if (!num) return NULL;
    num->sign = 1;
    num->size = 0;
    num->capacity = precision; 
    num->digits = calloc(num->capacity, sizeof(DIGITS_DTYPE));
    if (!num->digits) {
        free(num);
        return NULL;
    }
    return num;
}

APInt *apint_copy(APInt *num) {
    // A function for creating a copy of apint, note that the allocated space will depend on the currently set precision, this may result in loss of data.
    APInt *res = apint_init();
    res->sign = num->sign;
    // If the size of num i greater than the currect precision, loose the least significant digits.
    if (num->size > res->capacity) memcpy(res->digits, num->digits + (num->size - res->capacity), res->capacity * sizeof(DIGITS_DTYPE));
    // Else copy digits directly from num to res
    else memcpy(res->digits, num->digits, num->size * sizeof(DIGITS_DTYPE));
    res->size = num->size;
    return res;
}

APInt *apint_copy_ex(APInt *num, uint32_t precision) {
    APInt *res = apint_init_ex(precision);
    res->sign = num->sign;
    if (num->size > precision) memcpy(res->digits, num->digits + (num->size - precision), precision * sizeof(DIGITS_DTYPE));
    else memcpy(res->digits, num->digits, num->size * sizeof(DIGITS_DTYPE));
    res->size = (num->size > precision) ? precision : num->size;
    return res;
}

int apint_resize(APInt *num, uint32_t precision) {
    DIGITS_DTYPE *new_digits = calloc(precision, sizeof(DIGITS_DTYPE));
    if (!new_digits) return 0; // Memory allocation failed.
    // If the size of num i greater than the precision, loose the least significant digits.
    if (num->size > precision) {
        memmove(new_digits, num->digits + (num->size - precision), precision * sizeof(DIGITS_DTYPE));
    // Else copy digits directly from digits to new_digits
    } else memmove(new_digits, num->digits, num->size * sizeof(DIGITS_DTYPE));
    // Freeing the old digits
    free(num->digits);
    // Assigning the new
    num->digits = new_digits;
    num->size = precision;
    num->capacity = precision;
    apint_normalize(num);
    return 1;
}

APInt *apint_from_string(const char *str);

APInt *apint_from_int(long long num) {
    // Create an apint from a regular int
    APInt *res = apint_init();
    res->sign = num >= 0 ? 1 : -1;
    uint32_t intlen = num == 0 ? 1 : (uint32_t)log10(llabs(num)) + 1;
    if (num < 0) num = llabs(num);
    // If the length of the number is greater than the current precision, truncate the least significant digits.
    if (intlen > res->capacity) num /= (int)pow(10, intlen-res->capacity);
    int idx = 0;
    do {
        res->digits[idx] = num % 10;
        num /= 10;
        idx++;
        res->size++;
    } while (num);
    return res;
}

void apint_free(APInt *num) {
    free(num->digits);
    free(num);
}

int apint_is_zero(APInt *num) {
    return num->size == 0 || (num->size == 1 && num->digits[0] == 0);
}

void apint_normalize(APInt *num) {
    // Fix any issues with the representation
    //  - If zero, make sure size is 1
    //  - Remove leading zeroes from size
    while (num->size > 1 && num->digits[num->size - 1] == 0) num->size--;
    if (num->size == 0) {
        num->size = 1;
        num->digits[0] = 1;
    }
}

#endif

// APFloat helper functions
#if 1

APFloat *apfloat_init() {
    APFloat *num = (APFloat*)malloc(sizeof(APFloat));
    if (!num) return NULL;
    num->sign = 1;
    num->significand = apint_init();
    if (!num->significand) {
        free(num);
        return NULL;
    }
    num->exponent = 0;
    return num;
}

APFloat *apfloat_init_ex(uint32_t precision) {
    APFloat *num = (APFloat*)malloc(sizeof(APFloat));
    if (!num) return NULL;
    num->sign = 1;
    num->significand = apint_init_ex(precision);
    if (!num->significand) {
        free(num);
        return NULL;
    }
    num->exponent = 0;
    return num;
}

APFloat *apfloat_init_empty() {
    APFloat *num = (APFloat*)malloc(sizeof(APFloat));
    if (!num) return NULL;
    num->sign = 1;
    num->significand = NULL;
    num->exponent = 0;
    return num;
}


APFloat *apfloat_copy(APFloat *num) {
    APFloat *res = apfloat_init();
    res->sign = num->sign;
    res->significand = apint_copy(num->significand);
    res->exponent = num->exponent;
    return res;
}

APFloat *apfloat_copy_ex(APFloat *num, uint32_t precision) {
    APFloat *res = apfloat_init();
    res->sign = num->sign;
    res->significand = apint_copy_ex(num->significand, precision);
    res->exponent = num->exponent;
    return res;
}

int apfloat_resize(APFloat *num, uint32_t precision) {
    if (num->significand->size > precision) num->exponent += num->significand->size - precision;
    return apint_resize(num->significand, precision);
}

APFloat *apfloat_from_apint(APInt *num, int64_t exponent) {
    APFloat *res = (APFloat*)malloc(sizeof(APFloat));
    if (!res) return NULL;
    res->sign = num->sign;
    num->sign = 1;
    res->significand = num;
    res->exponent = exponent;
    return res;
}

void apfloat_free(APFloat *num) {
    if (num->significand) apint_free(num->significand);
    free(num);
}

void apfloat_normalize(APFloat *num) {
    apint_normalize(num->significand);
    num->significand->sign = 1;
    while (num->significand->digits[0] == 0) {
        apint_right_shift_inplace(num->significand, 1);
        num->exponent++;
    }
}

int apfloat_is_zero(APFloat *num) {
    return apint_is_zero(num->significand);
}

int apfloat_is_int(APFloat *num) {
    // 0001 -3 = 000.1 = 1.000
    // 000007 -2 = 00.0007 = 7000.00 
    if (num->exponent >= 0) return 1;
    for (uint64_t i = -num->exponent; i > 0; i--)
        if (num->significand->digits[i-1] != 0) return 0;
    return 1;
}

#endif

// APComplex helper functions
#if 1
#endif