#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include "logging_std_wrappers.h"

void *logging_malloc(size_t block_size, const char *variable_name) {
    assert(variable_name != NULL);
    void *allocated_block = malloc(block_size);
    if (allocated_block == NULL) log_error("Unable to allocate memory for %s\n", variable_name);
    return allocated_block;
}

FILE *logging_fopen(const char *filename, const char *mode) {
    FILE *file = fopen(filename, mode);
    if (file == NULL) log_error("Unable to find file \"%s\"\n", filename);
    return file;
}

size_t logging_fread(void *ptr, size_t size, size_t count, FILE *stream, const char *variable_name) {
    assert(variable_name != NULL);
    size_t result = fread(ptr, size, count, stream);
    if (result != count)
        log_error(feof(stream) ? "Unexpected end of file while reading %s\n" : "Unable to read %s\n", variable_name);
    return result;
}

size_t logging_fwrite(void *ptr, size_t size, size_t count, FILE *stream, const char *variable_name) {
    assert(variable_name != NULL);
    size_t result = fwrite(ptr, size, count, stream);
    if (result != count)
        log_error("Unable to write %s\n", variable_name);
    return result;
}
