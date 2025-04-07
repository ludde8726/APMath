#include <stdlib.h>
#include <math.h>

#include "APNumber.h"
#include "APHelpers.h"

static inline int get_exponent_digits(int64_t n) {
    static const int64_t powers_of_10[] = {
        1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 
        100000000, 1000000000, 10000000000, 100000000000, 
        1000000000000, 10000000000000, 100000000000000, 
        1000000000000000, 10000000000000000, 100000000000000000, 
        1000000000000000000
    };

    int count = 1;
    int sign = 0;
    if (n < 0) {
         n = -n; 
         sign = 1; 
    }
    for (int i = 18; i > 0; --i) {
        if (n >= powers_of_10[i]) {
            count = i + 1;
            break;
        }
    }
    return sign ? count + 1 : count;
}

char *apfloat_to_string(APFloat *num, enum PrintType print_type) {
    int scientific_notation = 0;
    if (print_type == SCIENTIFIC) scientific_notation = 1;
    if (print_type == BASE10) scientific_notation = 1;
    else if (print_type == REGULAR) scientific_notation = 0;
    else if (print_type == AUTO) {
        scientific_notation = (num->significand->size + num->exponent > 6 || num->significand->size + num->exponent < -4) && !apfloat_is_zero(num);
    }

    if (scientific_notation && print_type != BASE10) {
        if (apfloat_is_zero(num)) {
            // Hack to allow free to always be called on the return
            char *res = (char*)malloc(7 * sizeof(char));
            snprintf(res, 7, "0.0e+0");
            return res;
        }
        int64_t new_exponent = num->exponent + num->significand->size-1;
        // Allocate space for result buffer aswell as for the string representation of the numbers after the decimal
        // Null termination character takes up 1 byte of space.
        // e(+/-) takes up 2 bytes of space (assuming an extra space before the multiplication sign).
        // We also have to allocate space for the exponent.
        // Decimal point also takes up one byte of space.
        int extra_space = 4 + get_exponent_digits(new_exponent);
        if (num->sign == -1) extra_space++;

        char *after_decimal;

        if (num->significand->size <= 1) {
            // If we have a single digit number, just set the after decimal to 0
            after_decimal = (char*)malloc(2 * sizeof(char));
            after_decimal[0] = '0';
            after_decimal[1] = '\0';
            extra_space++;
        } else {
            // Create a string of all digits after the decimal point
            after_decimal = (char*)malloc(num->significand->size * sizeof(char));
            uint32_t index = 0;
            for (uint32_t i = num->significand->size-1; i > 0; i--) {
                after_decimal[index++] = '0' + num->significand->digits[i-1];
            }
            after_decimal[index++] = '\0';
        }

        char *res = (char*)malloc(num->significand->size * sizeof(char) + extra_space);

        // Print the result into res
        if (num->sign == -1) snprintf(res, num->significand->size + extra_space, "-%d.%se%+lld", num->significand->digits[num->significand->size-1], after_decimal, new_exponent);
        else snprintf(res, num->significand->size + extra_space, "%d.%se%+lld", num->significand->digits[num->significand->size-1], after_decimal, new_exponent);

        // Free the string of all digits after the decimal point
        free(after_decimal);

        return res;

    } else if (scientific_notation) {
        if (apfloat_is_zero(num)) {
            // Hack to allow free to always be called on the return
            char *res = (char*)malloc(11 * sizeof(char));
            snprintf(res, 11, "0.0 * 10^0");
            return res;
        }
        // Create new exponent, since we represent numbers as integer * 10^n but print them as float * 10^n
        int64_t new_exponent = num->exponent + num->significand->size-1;
        
        // Allocate space for result buffer aswell as for the string representation of the numbers after the decimal
        // Null termination character takes up 1 byte of space.
        // * 10^ takes up 6 bytes of space (assuming an extra space before the multiplication sign).
        // We also have to allocate space for the exponent.
        // Decimal point also takes up one byte of space.
        int extra_space = 8 + get_exponent_digits(new_exponent);
        if (num->sign == -1) extra_space++;

        char *after_decimal;

        if (num->significand->size <= 1) {
            // If we have a single digit number, just set the after decimal to 0
            after_decimal = (char*)malloc(2 * sizeof(char));
            after_decimal[0] = '0';
            after_decimal[1] = '\0';
            extra_space++;
        } else {
            // Create a string of all digits after the decimal point
            after_decimal = (char*)malloc(num->significand->size * sizeof(char));
            uint32_t index = 0;
            for (uint32_t i = num->significand->size-1; i > 0; i--) {
                after_decimal[index++] = '0' + num->significand->digits[i-1];
            }
            after_decimal[index++] = '\0';
        }

        char *res = (char*)malloc((num->significand->size + extra_space) * sizeof(char));
        
        // Print the result into res
        if (num->sign == -1) snprintf(res, num->significand->size + extra_space, "-%d.%s * 10^%lld", num->significand->digits[num->significand->size-1], after_decimal, new_exponent);
        else snprintf(res, num->significand->size + extra_space, "%d.%s * 10^%lld", num->significand->digits[num->significand->size-1], after_decimal, new_exponent);

        // Free the string of all digits after the decimal point
        free(after_decimal);

        return res;
    } else {
        if (apfloat_is_zero(num)) { 
            // Hack to allow free to always be called on the return
            char *res = (char*)malloc(4 * sizeof(char));
            snprintf(res, 4, "0.0");
            return res;
        } else if (num->exponent >= 0) {
            // If the exponent is grater than 0 we have trailing zeros
            int extra_space = num->exponent + 3; // We allocate space for the zeroes aswell as a .0 at the end
            if (num->sign == -1) extra_space++;
            char *res = (char*)malloc((num->significand->size + extra_space) * sizeof(char));

            uint32_t index = 0;
            if (num->sign == -1) res[index++] = '-';

            for (int64_t i = num->significand->size; i > -num->exponent; i--) {
                if (i > 0) res[index++] = '0' + num->significand->digits[i-1];
                else res[index++] = '0';
            }

            res[index++] = '.';
            res[index++] = '0';
            res[index++] = '\0';

            return res;
        } else if (num->significand->size + num->exponent <= 0) {
            // If the size plus the exponent (which is negative) is less than one, we have leading zeroes.
            int extra_space = llabs(num->significand->size + num->exponent) + 3; // We allocate space for the zeroes aswell as a 0. at the start
            if (num->sign == -1) extra_space++;
            char *res = (char*)malloc((num->significand->size + extra_space) * sizeof(char));

            uint32_t index = 0;
            if (num->sign == -1) res[index++] = '-';
            res[index++] = '0';
            res[index++] = '.';

            for (int64_t i = 0; i > num->significand->size + num->exponent; i--) res[index++] = '0';
            for (uint32_t i = num->significand->size; i > 0; i--) res[index++] = '0' + num->significand->digits[i-1];

            res[index++] = '\0';

            return res;
        } else {
            // We allocate space for the decimal and the null termination character.
            int extra_space = 2;
            if (num->sign == -1) extra_space++;
            char *res = (char*)malloc((num->significand->size + extra_space) * sizeof(char));

            uint32_t index = 0;
            uint32_t decimal_index = num->significand->size + num->exponent;

            if (num->sign == -1) {
                res[index++] = '-';
                decimal_index++;
            }

            for (uint32_t i = num->significand->size; i > 0; i--) {
                if (index == decimal_index) res[index++] = '.';
                res[index++] = '0' + num->significand->digits[i-1];
            }

            res[index++] = '\0';

            return res;
        }
    }

   perror("UNREACHABLE");
}

void print_apfloat(APFloat *num, enum PrintType print_type) {
    char *str_repr = apfloat_to_string(num, print_type);
    printf("%s\n", str_repr);
    free(str_repr);
}