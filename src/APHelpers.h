#include "APNumber.h"

enum PrintType {
    AUTO,
    SCIENTIFIC,
    BASE10,
    REGULAR,
};

static inline int get_exponent_digits(int64_t n);

// Converts an APFloat into a string. 
// The output format is determined by the specified PrintType:
// - AUTO: Chooses between regular and scientific notation based on the number's magnitude.
// - SCIENTIFIC: Forces scientific notation (e.g., 1.23e+10).
// - BASE10: Forces scientific notation but written as f * 10^n (e.g., 1.23 * 10^10).
// - REGULAR: Uses standard decimal notation without exponentiation.
// The function returns a dynamically allocated string that must be freed by the caller.
char *apfloat_to_string(APFloat *num, enum PrintType print_type);

// Prints an APFloat. 
// The output format is determined by the specified PrintType:
// - AUTO: Chooses between regular and scientific notation based on the number's magnitude.
// - SCIENTIFIC: Forces scientific notation (e.g., 1.23e+10).
// - BASE10: Forces scientific notation but written as f * 10^n (e.g., 1.23 * 10^10).
// - REGULAR: Uses standard decimal notation without exponentiation.
void print_apfloat(APFloat *num, enum PrintType print_type);
