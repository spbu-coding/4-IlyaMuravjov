#ifndef TASK_4_BUFFER_H
#define TASK_4_BUFFER_H

#include <stdint.h>
#include "byte.h"

byte_t read_byte_from_buffer(byte_t **buffer_head);
uint32_t read_le_uint32_from_buffer(byte_t **buffer_head);
int32_t read_le_int32_from_buffer(byte_t **buffer_head);
uint16_t read_le_uint16_from_buffer(byte_t **buffer_head);
void write_byte_to_buffer(byte_t byte, byte_t **buffer_head);
void write_le_uint32_to_buffer(uint32_t value, byte_t **buffer_head);
void write_le_int32_to_buffer(int32_t value, byte_t **buffer_head);
void write_le_uint16_to_buffer(uint16_t value, byte_t **buffer_head);

#endif // TASK_4_BUFFER_H
