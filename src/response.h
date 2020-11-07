#ifndef TASK_4_RESPONSE_H
#define TASK_4_RESPONSE_H

#include <stdbool.h>
#include <stdio.h>

typedef enum {
    SUCCESS,
    INVALID_VALUE,
    UNSUPPORTED_VALUE,
    OUT_OF_MEMORY_ERROR,
    IO_ERROR,
    FILE_NOT_FOUND,
    EOF_ERROR
} RESPONSE;

bool is_success(RESPONSE response);
bool is_error(RESPONSE response);

RESPONSE file_error_to_response(FILE *file);

#endif // TASK_4_RESPONSE_H