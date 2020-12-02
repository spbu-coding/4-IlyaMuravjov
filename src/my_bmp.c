#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <inttypes.h>
#include "my_bmp.h"
#include "buffer.h"
#include "logging_std_wrappers.h"

static const uint16_t MY_BMP_SUPPORTED_FORMAT_IDENTIFIER = 0x4D42;
#define MY_BMP_FILE_HEADER_SIZE ((uint32_t) 14)
#define MY_BMP_SUPPORTED_INFO_HEADER_SIZE ((uint32_t) 40)
static const uint32_t MY_BMP_SUPPORTED_BMP_HEADER_SIZE = MY_BMP_FILE_HEADER_SIZE + MY_BMP_SUPPORTED_INFO_HEADER_SIZE;
static const uint16_t MY_BMP_VALID_PLANES = 1;
static const uint32_t MY_BMP_SUPPORTED_COMPRESSION_METHOD = 0;
static const uint32_t MY_BMP_COLORS_IN_8_BIT_PALETTE = 256;
static const size_t MY_BMP_PALETTE_COLOR_SIZE = 4;

uint32_t get_my_bmp_colors_in_palette(MY_BMP *bmp) {
    assert(bmp != NULL);
    switch (bmp->header.bits_per_pixel) {
        case 8:
            return MY_BMP_COLORS_IN_8_BIT_PALETTE;
        case 24:
            return 0;
        default:
            assert(false);
    }
}

size_t get_my_bmp_palette_size(MY_BMP *bmp) {
    assert(bmp != NULL);
    return get_my_bmp_colors_in_palette(bmp) * MY_BMP_PALETTE_COLOR_SIZE;
}

int32_t int32_abs(int32_t x) {
    return x > 0 ? x : -x;
}

uint32_t get_my_bmp_abs_width(MY_BMP *bmp) {
    assert(bmp != NULL);
    return int32_abs(bmp->header.width);
}

uint32_t get_my_bmp_abs_height(MY_BMP *bmp) {
    assert(bmp != NULL);
    return int32_abs(bmp->header.height);
}

void destroy_my_bmp(MY_BMP *bmp) {
    if (bmp == NULL) return;
    free(bmp->palette);
    free(bmp->pixels);
}

RESPONSE read_my_bmp_header(MY_BMP_HEADER *header, FILE *file) {
    assert(header != NULL);
    assert(file != NULL);
    byte_t buffer[MY_BMP_SUPPORTED_BMP_HEADER_SIZE];
    byte_t *buffer_head = buffer;
    if (logging_fread(buffer, sizeof(buffer), 1, file, "bmp header") != 1)
        return file_error_to_response(file);
    header->format_identifier = read_next_le_uint16_from_buffer(&buffer_head);
    if (header->format_identifier != MY_BMP_SUPPORTED_FORMAT_IDENTIFIER) {
        log_error("Unsupported file format. Expected 0x%"PRIX16", but found 0x%"PRIX16"\n",
                  MY_BMP_SUPPORTED_FORMAT_IDENTIFIER, header->format_identifier);
        return UNSUPPORTED_VALUE;
    }
    header->file_size = read_next_le_uint32_from_buffer(&buffer_head);
    for (size_t i = 0; i < MY_BMP_RESERVED_COUNT; i++) {
        header->reserved[i] = read_next_le_uint16_from_buffer(&buffer_head);
        if (header->reserved[i] != 0) {
            log_error("Invalid reserved field #%zu. Expected 0, but found 0x%"PRIX16"\n", i + 1, header->reserved[i]);
            return INVALID_VALUE;
        }
    }
    header->pixel_array_offset = read_next_le_uint32_from_buffer(&buffer_head);
    header->info_header_size = read_next_le_uint32_from_buffer(&buffer_head);
    if (header->info_header_size != MY_BMP_SUPPORTED_INFO_HEADER_SIZE) {
        log_error("Unsupported info header size. Expected 0x%"PRIX32", but found 0x%"PRIX32"\n",
                  MY_BMP_SUPPORTED_INFO_HEADER_SIZE, header->info_header_size);
        return UNSUPPORTED_VALUE;
    }
    header->width = read_next_le_int32_from_buffer(&buffer_head);
    header->height = read_next_le_int32_from_buffer(&buffer_head);
    header->planes = read_next_le_uint16_from_buffer(&buffer_head);
    if (header->planes != MY_BMP_VALID_PLANES) {
        log_error("Invalid number of planes. Expected %"PRIu16", but found %"PRIu16"\n", MY_BMP_VALID_PLANES, header->planes);
        return INVALID_VALUE;
    }
    header->bits_per_pixel = read_next_le_uint16_from_buffer(&buffer_head);
    if (header->bits_per_pixel != 8 && header->bits_per_pixel != 24) {
        log_error("Unsupported number of bits per pixel. Expected 8 or 24, but found %"PRIu16"\n", header->bits_per_pixel);
        return UNSUPPORTED_VALUE;
    }
    header->compression_method = read_next_le_uint32_from_buffer(&buffer_head);
    if (header->compression_method != MY_BMP_SUPPORTED_COMPRESSION_METHOD) {
        log_error("Unsupported compression method. Expected %"PRIu32", but found %"PRIu32"\n",
                  MY_BMP_SUPPORTED_COMPRESSION_METHOD, header->compression_method);
        return UNSUPPORTED_VALUE;
    }
    header->image_size = read_next_le_uint32_from_buffer(&buffer_head);
    header->horizontal_resolution = read_next_le_int32_from_buffer(&buffer_head);
    header->vertical_resolution = read_next_le_int32_from_buffer(&buffer_head);
    header->colors_used = read_next_le_uint32_from_buffer(&buffer_head);
    header->important_colors = read_next_le_uint32_from_buffer(&buffer_head);
    return SUCCESS;
}

RESPONSE write_my_bmp_header(MY_BMP_HEADER *header, FILE *file) {
    assert(header != NULL);
    assert(file != NULL);
    byte_t buffer[MY_BMP_SUPPORTED_BMP_HEADER_SIZE];
    byte_t *buffer_head = buffer;
    write_next_le_uint16_to_buffer(header->format_identifier, &buffer_head);
    write_next_le_uint32_to_buffer(header->file_size, &buffer_head);
    for (size_t i = 0; i < MY_BMP_RESERVED_COUNT; i++)
        write_next_le_uint16_to_buffer(header->reserved[i], &buffer_head);
    write_next_le_uint32_to_buffer(header->pixel_array_offset, &buffer_head);
    write_next_le_uint32_to_buffer(header->info_header_size, &buffer_head);
    write_next_le_int32_to_buffer(header->width, &buffer_head);
    write_next_le_int32_to_buffer(header->height, &buffer_head);
    write_next_le_uint16_to_buffer(header->planes, &buffer_head);
    write_next_le_uint16_to_buffer(header->bits_per_pixel, &buffer_head);
    write_next_le_uint32_to_buffer(header->compression_method, &buffer_head);
    write_next_le_uint32_to_buffer(header->image_size, &buffer_head);
    write_next_le_int32_to_buffer(header->horizontal_resolution, &buffer_head);
    write_next_le_int32_to_buffer(header->vertical_resolution, &buffer_head);
    write_next_le_uint32_to_buffer(header->colors_used, &buffer_head);
    write_next_le_uint32_to_buffer(header->important_colors, &buffer_head);
    if (logging_fwrite(buffer, sizeof(buffer), 1, file, "bmp header") != 1)
        return file_error_to_response(file);
    return SUCCESS;
}

RESPONSE allocate_and_read_my_bmp_palette(MY_BMP *bmp, FILE *file) {
    assert(bmp != NULL);
    assert(file != NULL);
    size_t palette_size = get_my_bmp_palette_size(bmp);
    if (palette_size == 0) bmp->palette = NULL;
    else {
        bmp->palette = logging_malloc(palette_size, "palette");
        if (bmp->palette == NULL) return OUT_OF_MEMORY_ERROR;
        if (logging_fread(bmp->palette, 1, palette_size, file, "palette") != palette_size)
            return file_error_to_response(file);
    }
    return SUCCESS;
}

RESPONSE write_my_bmp_palette(MY_BMP *bmp, FILE *file) {
    assert(bmp != NULL);
    assert(file != NULL);
    size_t palette_size = get_my_bmp_palette_size(bmp);
    if (palette_size != 0)
        if (logging_fwrite(bmp->palette, 1, palette_size, file, "palette") != palette_size)
            return file_error_to_response(file);
    return SUCCESS;
}

RESPONSE allocate_and_read_my_bmp_pixels(MY_BMP *bmp, FILE *file) {
    assert(bmp != NULL);
    assert(file != NULL);
    bmp->pixels = logging_malloc(bmp->header.image_size, "pixels");
    if (bmp->pixels == NULL) return OUT_OF_MEMORY_ERROR;
    if (logging_fread(bmp->pixels, 1, bmp->header.image_size, file, "pixels") != bmp->header.image_size)
        return file_error_to_response(file);
    return SUCCESS;
}

RESPONSE write_my_bmp_pixels(MY_BMP *bmp, FILE *file) {
    assert(bmp != NULL);
    assert(file != NULL);
    if (fwrite(bmp->pixels, 1, bmp->header.image_size, file) != bmp->header.image_size)
        return file_error_to_response(file);
    return SUCCESS;
}

RESPONSE read_my_bmp(MY_BMP *bmp, const char *filename) {
    assert(bmp != NULL);
    assert(filename != NULL);
    memset(bmp, 0, sizeof(MY_BMP));
    FILE *file = logging_fopen(filename, "rb");
    if (file == NULL) return FILE_NOT_FOUND;
    RESPONSE response;
    if (is_success(response = read_my_bmp_header(&bmp->header, file)))
        if (is_success(response = allocate_and_read_my_bmp_palette(bmp, file)))
            response = allocate_and_read_my_bmp_pixels(bmp, file);
    if (is_error(response)) destroy_my_bmp(bmp);
    fclose(file);
    return response;
}

RESPONSE write_my_bmp(MY_BMP *bmp, const char *filename) {
    assert(bmp != NULL);
    assert(filename != NULL);
    FILE *file = logging_fopen(filename, "wb");
    if (file == NULL) return FILE_NOT_FOUND;
    RESPONSE response;
    if (is_success(response = write_my_bmp_header(&bmp->header, file)))
        if (is_success(response = write_my_bmp_palette(bmp, file)))
            response = write_my_bmp_pixels(bmp, file);
    fclose(file);
    return response;
}

bool my_bmp_has_palette(MY_BMP *bmp) {
    assert(bmp != NULL);
    return bmp->palette != NULL;
}

void assert_pixel_pos_in_my_bmp_bounds(MY_BMP *bmp, PIXEL_POS pixel_pos) {
    assert(bmp != NULL);
    assert(pixel_pos.x < get_my_bmp_abs_width(bmp));
    assert(pixel_pos.y < get_my_bmp_abs_height(bmp));
}

byte_t *get_my_bmp_pixel_data_ptr(MY_BMP *bmp, PIXEL_POS pixel_pos) {
    assert(bmp != NULL);
    assert_pixel_pos_in_my_bmp_bounds(bmp, pixel_pos);
    if (bmp->header.width < 0) pixel_pos.x = get_my_bmp_abs_width(bmp) - pixel_pos.x - 1;
    if (bmp->header.height < 0) pixel_pos.y = get_my_bmp_abs_height(bmp) - pixel_pos.y - 1;
    uint16_t bytes_per_pixel = bmp->header.bits_per_pixel / 8;
    uint32_t bytes_per_row = bmp->header.image_size / get_my_bmp_abs_height(bmp);
    return bmp->pixels + (pixel_pos.y * bytes_per_row + pixel_pos.x * bytes_per_pixel);
}

COLOR *get_my_bmp_palette_color_ptr(MY_BMP *bmp, uint32_t index) {
    assert(bmp != NULL);
    assert(my_bmp_has_palette(bmp));
    assert(index < get_my_bmp_colors_in_palette(bmp));
    return (COLOR *) (bmp->palette + index * MY_BMP_PALETTE_COLOR_SIZE);
}

byte_t *get_my_bmp_pixel_index_ptr(MY_BMP *bmp, PIXEL_POS pixel_pos) {
    assert(bmp != NULL);
    assert(my_bmp_has_palette(bmp));
    assert(bmp->header.bits_per_pixel == 8);
    assert_pixel_pos_in_my_bmp_bounds(bmp, pixel_pos);
    return get_my_bmp_pixel_data_ptr(bmp, pixel_pos);
}

COLOR *get_my_bmp_pixel_color_ptr(MY_BMP *bmp, PIXEL_POS pixel_pos) {
    assert(bmp != NULL);
    assert_pixel_pos_in_my_bmp_bounds(bmp, pixel_pos);
    switch (bmp->header.bits_per_pixel) {
        case 8:
            return get_my_bmp_palette_color_ptr(bmp, *get_my_bmp_pixel_index_ptr(bmp, pixel_pos));
        case 24:
            return (COLOR *) get_my_bmp_pixel_data_ptr(bmp, pixel_pos);
        default:
            assert(false);
    }
}

PIXEL_POS_ITERATOR get_my_bmp_pixel_pos_iterator(MY_BMP *bmp) {
    assert(bmp != NULL);
    return (PIXEL_POS_ITERATOR) {
            {0, 0},
            get_my_bmp_abs_width(bmp),
            get_my_bmp_abs_height(bmp)
    };
}

bool has_next_pixel_pos(PIXEL_POS_ITERATOR *iterator) {
    assert(iterator != NULL);
    return iterator->next.y < iterator->height;
}

PIXEL_POS get_next_pixel_pos(PIXEL_POS_ITERATOR *iterator) {
    assert(iterator != NULL);
    assert(has_next_pixel_pos(iterator));
    PIXEL_POS current = iterator->next;
    iterator->next.x++;
    if (iterator->next.x == iterator->width) {
        iterator->next.x = 0;
        iterator->next.y++;
    }
    return current;
}
