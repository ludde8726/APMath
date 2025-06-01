#include <stdlib.h>
#include <math.h>

#include "APNumber.h"
#include "APHelpers.h"
#include "APError.h"

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

char *apfloat_to_string(APFloat *x, enum PrintType print_type) {
    int scientific_notation = 0;
    if (print_type == SCIENTIFIC) scientific_notation = 1;
    if (print_type == BASE10) scientific_notation = 1;
    else if (print_type == REGULAR) scientific_notation = 0;
    else if (print_type == AUTO) {
        scientific_notation = (APF_SZ(x) + x->exponent > 6 || APF_SZ(x) + x->exponent < -4) && !apfloat_is_zero(x);
    }

    if (scientific_notation && print_type != BASE10) {
        if (apfloat_is_zero(x)) {
            // Hack to allow free to always be called on the return
            char *res = (char*)malloc(8 * sizeof(char));
            if (!res) {
                apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)(8 * sizeof(char)));
                return NULL;
            }
            snprintf(res, 8, "0.0e+00");
            return res;
        }
        int64_t new_exponent = x->exponent + APF_SZ(x)-1;
        // Allocate space for result buffer aswell as for the string representation of the numbers after the decimal
        // Null termination character takes up 1 byte of space.
        // e(+/-) takes up 2 bytes of space (assuming an extra space before the multiplication sign).
        // We also have to allocate space for the exponent.
        // Decimal point also takes up one byte of space.

        int exponent_digits = get_exponent_digits(new_exponent);
        if (exponent_digits == 1) exponent_digits++;

        int extra_space = 4 + exponent_digits+10;
        if (x->sign == -1) extra_space++;

        char *after_decimal;

        if (APF_SZ(x) <= 1) {
            // If we have a single digit number, just set the after decimal to 0
            after_decimal = (char*)malloc(2 * sizeof(char));
            if (!after_decimal) {
                apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)(2 * sizeof(char)));
                return NULL;
            }
            after_decimal[0] = '0';
            after_decimal[1] = '\0';
            extra_space++;
        } else {
            // Create a string of all digits after the decimal point
            after_decimal = (char*)malloc(APF_SZ(x) * sizeof(char));
            if (!after_decimal) {
                apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)(APF_SZ(x) * sizeof(char)));
                return NULL;
            }
            uint32_t index = 0;
            for (uint32_t i = APF_SZ(x)-1; i > 0; i--) {
                after_decimal[index++] = '0' + APF_DIG(x)[i-1];
            }
            after_decimal[index++] = '\0';
        }

        char *res = (char*)malloc((APF_SZ(x) + extra_space) * sizeof(char));
        if (!res) {
            free(after_decimal);
            apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)((APF_SZ(x) + extra_space) * sizeof(char)));
            return NULL;
        }

        // Print the result into res
        if (x->sign == -1) snprintf(res, APF_SZ(x) + extra_space, "-%d.%se%+03lld", APF_DIG(x)[APF_SZ(x)-1], after_decimal, new_exponent);
        else snprintf(res, APF_SZ(x) + extra_space, "%d.%se%+03lld", APF_DIG(x)[APF_SZ(x)-1], after_decimal, new_exponent);

        // Free the string of all digits after the decimal point
        free(after_decimal);

        return res;

    } else if (scientific_notation) {
        if (apfloat_is_zero(x)) {
            // Hack to allow free to always be called on the return
            char *res = (char*)malloc(11 * sizeof(char));
            if (!res) {
                apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)(11 * sizeof(char)));
                return NULL;
            }
            snprintf(res, 11, "0.0 * 10^0");
            return res;
        }
        // Create new exponent, since we represent numbers as integer * 10^n but print them as float * 10^n
        int64_t new_exponent = x->exponent + APF_SZ(x)-1;
        
        // Allocate space for result buffer aswell as for the string representation of the numbers after the decimal
        // Null termination character takes up 1 byte of space.
        // * 10^ takes up 6 bytes of space (assuming an extra space before the multiplication sign).
        // We also have to allocate space for the exponent.
        // Decimal point also takes up one byte of space.
        int extra_space = 8 + get_exponent_digits(new_exponent);
        if (x->sign == -1) extra_space++;

        char *after_decimal;

        if (APF_SZ(x) <= 1) {
            // If we have a single digit number, just set the after decimal to 0
            after_decimal = (char*)malloc(2 * sizeof(char));
            if (!after_decimal) {
                apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)(2 * sizeof(char)));
                return NULL;
            }
            after_decimal[0] = '0';
            after_decimal[1] = '\0';
            extra_space++;
        } else {
            // Create a string of all digits after the decimal point
            after_decimal = (char*)malloc(APF_SZ(x) * sizeof(char));
            if (!after_decimal) {
                apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)(APF_SZ(x) * sizeof(char)));
                return NULL;
            }
            uint32_t index = 0;
            for (uint32_t i = APF_SZ(x)-1; i > 0; i--) {
                after_decimal[index++] = '0' + APF_DIG(x)[i-1];
            }
            after_decimal[index++] = '\0';
        }

        char *res = (char*)malloc((APF_SZ(x) + extra_space) * sizeof(char));
        if (!res) {
            free(after_decimal);
            apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)((APF_SZ(x) + extra_space) * sizeof(char)));
            return NULL;
        }
        
        // Print the result into res
        if (x->sign == -1) snprintf(res, APF_SZ(x) + extra_space, "-%d.%s * 10^%lld", APF_DIG(x)[APF_SZ(x)-1], after_decimal, new_exponent);
        else snprintf(res, APF_SZ(x) + extra_space, "%d.%s * 10^%lld", APF_DIG(x)[APF_SZ(x)-1], after_decimal, new_exponent);

        // Free the string of all digits after the decimal point
        free(after_decimal);

        return res;
    } else {
        if (apfloat_is_zero(x)) { 
            // Hack to allow free to always be called on the return
            char *res = (char*)malloc(4 * sizeof(char));
            if (!res) {
                apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)(4 * sizeof(char)));
                return NULL;
            }
            snprintf(res, 4, "0.0");
            return res;
        } else if (x->exponent >= 0) {
            // If the exponent is grater than 0 we have trailing zeros
            int extra_space = x->exponent + 3; // We allocate space for the zeroes aswell as a .0 at the end
            if (x->sign == -1) extra_space++;
            char *res = (char*)malloc((APF_SZ(x) + extra_space) * sizeof(char));
            if (!res) {
                apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)((APF_SZ(x) + extra_space) * sizeof(char)));
                return NULL;
            }

            uint32_t index = 0;
            if (x->sign == -1) res[index++] = '-';

            for (int64_t i = APF_SZ(x); i > -x->exponent; i--) {
                if (i > 0) res[index++] = '0' + APF_DIG(x)[i-1];
                else res[index++] = '0';
            }

            res[index++] = '.';
            res[index++] = '0';
            res[index++] = '\0';

            return res;
        } else if (APF_SZ(x) + x->exponent <= 0) {
            // If the size plus the exponent (which is negative) is less than one, we have leading zeroes.
            int extra_space = llabs(APF_SZ(x) + x->exponent) + 3; // We allocate space for the zeroes aswell as a 0. at the start
            if (x->sign == -1) extra_space++;
            char *res = (char*)malloc((APF_SZ(x) + extra_space) * sizeof(char));
            if (!res) {
                apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)((APF_SZ(x) + extra_space) * sizeof(char)));
                return NULL;
            }

            uint32_t index = 0;
            if (x->sign == -1) res[index++] = '-';
            res[index++] = '0';
            res[index++] = '.';

            for (int64_t i = 0; i > APF_SZ(x) + x->exponent; i--) res[index++] = '0';
            for (uint32_t i = APF_SZ(x); i > 0; i--) res[index++] = '0' + APF_DIG(x)[i-1];

            res[index++] = '\0';

            return res;
        } else {
            // We allocate space for the decimal and the null termination character.
            int extra_space = 2;
            if (x->sign == -1) extra_space++;
            char *res = (char*)malloc((APF_SZ(x) + extra_space) * sizeof(char));
            if (!res) {
                apm_set_error_ex(APM_ERROR_OUT_OF_MEMORY, "Could not allocate %u bytes for string in `apfloat_to_string`.\n", (uint32_t)((APF_SZ(x) + extra_space) * sizeof(char)));
                return NULL;
            }

            uint32_t index = 0;
            uint32_t decimal_index = APF_SZ(x) + x->exponent;

            if (x->sign == -1) {
                res[index++] = '-';
                decimal_index++;
            }

            for (uint32_t i = APF_SZ(x); i > 0; i--) {
                if (index == decimal_index) res[index++] = '.';
                res[index++] = '0' + APF_DIG(x)[i-1];
            }

            res[index++] = '\0';

            return res;
        }
    }

   perror("UNREACHABLE");
}

void print_apfloat(APFloat *x, enum PrintType print_type) {
    char *str_repr = apfloat_to_string(x, print_type);
    if (!str_repr) {
        printf("null");
        return;
    }
    printf("%s\n", str_repr);
    free(str_repr);
}
