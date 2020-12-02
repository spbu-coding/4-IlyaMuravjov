#ifndef TASK_4_MY_BMP_H
#define TASK_4_MY_BMP_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "byte.h"
#include "response.h"

#define MY_BMP_RESERVED_COUNT 2

typedef struct {
    uint16_t format_identifier;
    uint32_t file_size;
    uint16_t reserved[MY_BMP_RESERVED_COUNT];
    uint32_t pixel_array_offset;
    uint32_t info_header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression_method;
    uint32_t image_size;
    int32_t horizontal_resolution;
    int32_t vertical_resolution;
    uint32_t colors_used;
    uint32_t important_colors;
} MY_BMP_HEADER;

typedef struct {
    byte_t blue;
    byte_t green;
    byte_t red;
} COLOR;

typedef struct {
    MY_BMP_HEADER header;
    byte_t *palette;
    byte_t *pixels;
} MY_BMP;

typedef struct {
    uint32_t x, y;
} PIXEL_POS;

typedef struct {
    PIXEL_POS next;
    uint32_t width;
    uint32_t height;
} PIXEL_POS_ITERATOR;

uint32_t get_my_bmp_colors_in_palette(MY_BMP *bmp);
uint32_t get_my_bmp_width(MY_BMP *bmp);
uint32_t get_my_bmp_abs_height(MY_BMP *bmp);
bool my_bmp_has_palette(MY_BMP *bmp);

void destroy_my_bmp(MY_BMP *bmp);
RESPONSE read_my_bmp(MY_BMP *bmp, const char *filename);
RESPONSE write_my_bmp(MY_BMP *bmp, const char *filename);

byte_t *get_my_bmp_pixel_data_ptr(MY_BMP *bmp, PIXEL_POS pixel_pos);
COLOR *get_my_bmp_palette_color_ptr(MY_BMP *bmp, uint32_t index);
byte_t *get_my_bmp_pixel_index_ptr(MY_BMP *bmp, PIXEL_POS pixel_pos);
COLOR *get_my_bmp_pixel_color_ptr(MY_BMP *bmp, PIXEL_POS pixel_pos);

PIXEL_POS_ITERATOR get_my_bmp_pixel_pos_iterator(MY_BMP *bmp);
bool has_next_pixel_pos(PIXEL_POS_ITERATOR *iterator);
PIXEL_POS get_next_pixel_pos(PIXEL_POS_ITERATOR *iterator);

#endif // TASK_4_MY_BMP_H
