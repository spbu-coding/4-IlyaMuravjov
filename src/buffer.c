#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include "buffer.h"

byte_t read_byte_from_buffer(byte_t **buffer_head) {
    assert(buffer_head != NULL);
    byte_t byte = **buffer_head;
    (*buffer_head)++;
    return byte;
}

uint32_t read_le_uint32_from_buffer(byte_t **buffer_head) {
    assert(buffer_head != NULL);
    return ((uint32_t) read_byte_from_buffer(buffer_head)) |
           ((uint32_t) read_byte_from_buffer(buffer_head) << 8u) |
           ((uint32_t) read_byte_from_buffer(buffer_head) << (2u * 8u)) |
           ((uint32_t) read_byte_from_buffer(buffer_head) << (3u * 8u));
}

int32_t read_le_int32_from_buffer(byte_t **buffer_head) {
    assert(buffer_head != NULL);
    return (int32_t) read_le_uint32_from_buffer(buffer_head);
}

uint16_t read_le_uint16_from_buffer(byte_t **buffer_head) {
    assert(buffer_head != NULL);
    return (uint16_t) read_byte_from_buffer(buffer_head) |
           (uint16_t) ((uint16_t) read_byte_from_buffer(buffer_head) << 8u);
}

void write_byte_to_buffer(byte_t byte, byte_t **buffer_head) {
    **buffer_head = byte;
    (*buffer_head)++;
}

void write_le_uint32_to_buffer(uint32_t value, byte_t **buffer_head) {
    assert(buffer_head != NULL);
    write_byte_to_buffer((value & 0x000000ffu), buffer_head);
    write_byte_to_buffer((value & 0x0000ff00u) >> 8u, buffer_head);
    write_byte_to_buffer((value & 0x00ff0000u) >> (2u * 8u), buffer_head);
    write_byte_to_buffer((value & 0xff000000u) >> (3u * 8u), buffer_head);
}

void write_le_int32_to_buffer(int32_t value, byte_t **buffer_head) {
    assert(buffer_head != NULL);
    write_le_uint32_to_buffer((uint32_t) value, buffer_head);
}

void write_le_uint16_to_buffer(uint16_t value, byte_t **buffer_head) {
    assert(buffer_head != NULL);
    write_byte_to_buffer((value & 0x00ffu), buffer_head);
    write_byte_to_buffer((value & 0xff00u) >> 8u, buffer_head);
}
