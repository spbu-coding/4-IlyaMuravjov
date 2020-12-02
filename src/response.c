#include <assert.h>
#include <stddef.h>
#include "response.h"

bool is_success(RESPONSE response) {
    return response == SUCCESS;
}

bool is_error(RESPONSE response) {
    return response != SUCCESS;
}

RESPONSE file_error_to_response(FILE *file) {
    assert(file != NULL);
    return feof(file) ? EOF_ERROR : IO_ERROR;
}
