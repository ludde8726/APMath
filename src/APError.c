#include <stdlib.h>

#include "APError.h"

static struct APM_Error APM_LastError = { 
    .error = NO_ERROR,
    .message = NULL
 };

struct APM_Error* apm_get_error(void) {
    if (APM_LastError.error != NO_ERROR) return &APM_LastError;
    return NULL;
}

void apm_set_error(enum APM_ErrorType error_type, char *error_message) {
    APM_LastError.error = error_type;
    APM_LastError.message = error_message;
}