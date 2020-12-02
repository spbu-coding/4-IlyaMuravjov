#ifndef TASK_4_LOGGING_STD_WRAPPERS_H
#define TASK_4_LOGGING_STD_WRAPPERS_H

#include "stddef.h"

#define log_error(...) (fprintf(stderr, __VA_ARGS__))

void *logging_malloc(size_t block_size, const char *variable_name);
FILE *logging_fopen(const char *filename, const char *mode);
size_t logging_fread(void *ptr, size_t size, size_t count, FILE *stream, const char *variable_name);
size_t logging_fwrite(void *ptr, size_t size, size_t count, FILE *stream, const char *variable_name);

#endif // TASK_4_LOGGING_STD_WRAPPERS_H
