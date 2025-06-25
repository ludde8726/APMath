#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define PRINTF_LIKE_WARNINGS(string_idx, arguments_idx_start) __attribute__((format(printf, string_idx, arguments_idx_start)))
#else
#define PRINTF_LIKE_WARNINGS(string_idx, arguments_idx_start)
#endif

enum APM_ErrorType {
    APM_ERROR_NONE,
    APM_ERROR_OUT_OF_MEMORY,
    APM_ERROR_DIVISION_BY_ZERO,
    APM_ERROR_ROUNDING,
    APM_ERROR_OVERFLOW
};

enum APM_ErrorHandle {
    APM_ERROR_HANDLE_NONE,
    APM_ERROR_HANDLE_LOG,
    APM_ERROR_HANDLE_FATAL
};

struct APM_Error {
    enum APM_ErrorType error;
    char *message;
};

char *apm_get_error(void);
struct APM_Error apm_get_error_ex(void);
void apm_set_error(enum APM_ErrorType error_type, char *error_message);
void apm_set_error_ex(enum APM_ErrorType error_type, const char *error_message, ...) PRINTF_LIKE_WARNINGS(2, 3);

void apm_set_error_handle(enum APM_ErrorHandle error_handle);
