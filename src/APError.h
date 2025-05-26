#pragma once

#define REPORT_ERROR_OOM(x) do { apm_set_error(OUT_OF_MEMORY_ERROR, "MemoryError: Could not allocate memory for number."); return x; } while (0)

enum APM_ErrorType {
    NO_ERROR,
    OUT_OF_MEMORY_ERROR,
    DIVISION_BY_ZERO_ERROR,
    ROUNDING_ERROR,
    OVERFLOW_ERROR
};

struct APM_Error {
    enum APM_ErrorType error;
    char *message;
};

struct APM_Error *apm_get_error(void);
void apm_set_error(enum APM_ErrorType error_type, char *error_message);
