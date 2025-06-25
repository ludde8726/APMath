#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "APError.h"

static struct APM_Error APM_LastError = { 
    .error = APM_ERROR_NONE,
    .message = NULL
};

static enum APM_ErrorHandle APM_CurrentErrorHandle = APM_ERROR_HANDLE_NONE;

char *apm_get_error(void) {
    static char error_msg[1024]; // Apparently it has to be static for it to be returned as a pointer through a function?
    switch (APM_LastError.error) {
        case APM_ERROR_NONE: return "";
        case APM_ERROR_OUT_OF_MEMORY:
            snprintf(error_msg, sizeof(error_msg), "Memory Error: %s", APM_LastError.message);
            return error_msg;
        case APM_ERROR_DIVISION_BY_ZERO:
            snprintf(error_msg, sizeof(error_msg), "Zero Division Error: %s", APM_LastError.message);
            return error_msg;
        case APM_ERROR_ROUNDING:
            snprintf(error_msg, sizeof(error_msg), "Rounding Error: %s", APM_LastError.message);
            return error_msg;
        case APM_ERROR_OVERFLOW:
            snprintf(error_msg, sizeof(error_msg), "Overflow Error: %s", APM_LastError.message);
            return error_msg;
    }
}

struct APM_Error apm_get_error_ex(void) {
    return APM_LastError;
}

void apm_set_error(enum APM_ErrorType error_type, char *error_message) {
    APM_LastError.error = error_type;
    if (APM_LastError.message) free(APM_LastError.message);
    APM_LastError.message = error_message;

    switch (APM_CurrentErrorHandle) {
        case APM_ERROR_HANDLE_NONE: return;
        case APM_ERROR_HANDLE_LOG:
            {
                printf("%s\n", apm_get_error());
                return;
            }
        case APM_ERROR_HANDLE_FATAL:
            {
                fprintf(stderr, "%s\n", apm_get_error());
                exit(1);
            }
    }
}

void apm_set_error_ex(enum APM_ErrorType error_type, const char *error_message, ...) {
    va_list args;
    va_start(args, error_message);

    va_list args_copy;
    va_copy(args_copy, args);
    int needed_size = vsnprintf(NULL, 0, error_message, args_copy);
    va_end(args_copy);

    char *formatted = malloc(needed_size + 1);
    if (!formatted) {
        va_end(args);
        fprintf(stderr, "Malloc failed in apm_set_error_ex\n");
        exit(1);
    }

    vsnprintf(formatted, needed_size + 1, error_message, args);
    va_end(args);

    APM_LastError.error = error_type;
    if (APM_LastError.message) free(APM_LastError.message);
    APM_LastError.message = formatted;
    
    switch (APM_CurrentErrorHandle) {
        case APM_ERROR_HANDLE_NONE: return;
        case APM_ERROR_HANDLE_LOG:
            {
                printf("%s\n", apm_get_error());
                return;
            }
        case APM_ERROR_HANDLE_FATAL:
            {
                fprintf(stderr, "%s\n", apm_get_error());
                exit(1);
            }
    }
}

void apm_set_error_handle(enum APM_ErrorHandle error_handle) {
    APM_CurrentErrorHandle = error_handle;
}
