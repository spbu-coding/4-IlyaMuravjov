#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include "buffer.h"

static const uint32_t BYTE_MASKS[] = {0x000000ffu, 0x0000ff00u, 0x00ff0000u, 0xff000000u};
#define BITS_PER_BYTE CHAR_BIT

byte_t read_next_byte_from_buffer(byte_t **buffer_head) {
    assert(buffer_head != NULL);
    byte_t byte = **buffer_head;
    (*buffer_head)++;
    return byte;
}

uint32_t read_next_le_uint32_from_buffer(byte_t **buffer_head) {
    assert(buffer_head != NULL);
    uint32_t result = 0;
    for (size_t i = 0; i < sizeof(uint32_t); i++)
        result |= ((uint32_t) read_next_byte_from_buffer(buffer_head)) << (i * BITS_PER_BYTE);
    return result;
}

int32_t read_next_le_int32_from_buffer(byte_t **buffer_head) {
    assert(buffer_head != NULL);
    return (int32_t) read_next_le_uint32_from_buffer(buffer_head);
}

uint16_t read_next_le_uint16_from_buffer(byte_t **buffer_head) {
    assert(buffer_head != NULL);
    uint16_t result = 0;
    for (size_t i = 0; i < sizeof(uint16_t); i++)
        result |= (uint16_t) ((uint16_t) read_next_byte_from_buffer(buffer_head) << (i * BITS_PER_BYTE));
    return result;
}

void write_next_byte_to_buffer(byte_t byte, byte_t **buffer_head) {
    assert(buffer_head != NULL);
    **buffer_head = byte;
    (*buffer_head)++;
}

void write_next_le_uint32_to_buffer(uint32_t value, byte_t **buffer_head) {
    assert(buffer_head != NULL);
    for (size_t i = 0; i < sizeof(uint32_t); i++)
        write_next_byte_to_buffer((value & BYTE_MASKS[i]) >> (i * BITS_PER_BYTE), buffer_head);
}

void write_next_le_int32_to_buffer(int32_t value, byte_t **buffer_head) {
    assert(buffer_head != NULL);
    write_next_le_uint32_to_buffer((uint32_t) value, buffer_head);
}

void write_next_le_uint16_to_buffer(uint16_t value, byte_t **buffer_head) {
    assert(buffer_head != NULL);
    for (size_t i = 0; i < sizeof(uint16_t); i++)
        write_next_byte_to_buffer((value & BYTE_MASKS[i]) >> (i * BITS_PER_BYTE), buffer_head);
}
