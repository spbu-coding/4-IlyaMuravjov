#include <stdio.h>
#include <memory.h>
#include <inttypes.h>
#include <assert.h>
#include "my_bmp.h"
#include "qdbmp.h"
#include "logging_std_wrappers.h"

static const int CONVERTER_GENERAL_ERROR_EXIT_CODE = -1;
static const int CONVERTER_INVALID_FILE_STRUCTURE_EXIT_CODE = -2;
static const int CONVERTER_QDBMP_ERROR_EXIT_CODE = -3;
static const int CONVERTER_INVALID_COMMAND_LINE_ARGS_EXIT_CODE = -4;

static const int CONVERTER_IMPLEMENTATION_TYPE_ARG_INDEX = 1;
static const int CONVERTER_INPUT_FILE_ARG_INDEX = 2;
static const int CONVERTER_OUTPUT_FILE_ARG_INDEX = 3;

static const int CONVERTER_REQUIRED_ARG_COUNT = 1 + 3;

void invert_color(COLOR *color) {
    assert(color != NULL);
    color->red = ~color->red;
    color->green = ~color->green;
    color->blue = ~color->blue;
}

void invert_my_bmp(MY_BMP *bmp) {
    assert(bmp != NULL);
    if (my_bmp_has_palette(bmp)) {
        uint32_t colors_in_palette = get_my_bmp_colors_in_palette(bmp);
        for (uint32_t i = 0; i < colors_in_palette; i++)
            invert_color(get_my_bmp_palette_color_ptr(bmp, i));
    } else {
        PIXEL_POS_ITERATOR pixel_pos_iterator = get_my_bmp_pixel_pos_iterator(bmp);
        while (has_next_pixel_pos(&pixel_pos_iterator))
            invert_color(get_my_bmp_pixel_color_ptr(bmp, get_next_pixel_pos(&pixel_pos_iterator)));
    }
}

void invert_qdbmp(BMP *bmp) {
    assert(bmp != NULL);
    UCHAR r, g, b;
    USHORT depth = BMP_GetDepth(bmp);
    if (BMP_GetError() != BMP_OK) return;
    switch (depth) {
        case 8: {
            for (int i = 0; i < 256; i++) {
                BMP_GetPaletteColor(bmp, i, &r, &g, &b);
                if (BMP_GetError() != BMP_OK) return;
                BMP_SetPaletteColor(bmp, i, ~r, ~g, ~b);
                if (BMP_GetError() != BMP_OK) return;
            }
            break;
        }
        case 24: {
            UINT width = BMP_GetWidth(bmp);
            if (BMP_GetError() != BMP_OK) return;
            UINT height = BMP_GetHeight(bmp);
            if (BMP_GetError() != BMP_OK) return;
            for (UINT y = 0; y < height; y++)
                for (UINT x = 0; x < width; x++) {
                    BMP_GetPixelRGB(bmp, x, y, &r, &g, &b);
                    if (BMP_GetError() != BMP_OK) return;
                    BMP_SetPixelRGB(bmp, x, y, ~r, ~g, ~b);
                    if (BMP_GetError() != BMP_OK) return;
                }
            break;
        }
        default:
            BMP_SetError(BMP_TYPE_MISMATCH);
    }
}

int main(int argc, char *argv[]) {
    if (argc != CONVERTER_REQUIRED_ARG_COUNT) {
        log_error("Invalid number of command line arguments. Expected %d arg(s), but found %d arg(s)\n", CONVERTER_REQUIRED_ARG_COUNT - 1, argc - 1);
        return CONVERTER_INVALID_COMMAND_LINE_ARGS_EXIT_CODE;
    }
    if (strcmp(argv[CONVERTER_IMPLEMENTATION_TYPE_ARG_INDEX], "--mine") == 0) {
        RESPONSE response;
        MY_BMP bmp;
        if (is_success(response = read_my_bmp(&bmp, argv[CONVERTER_INPUT_FILE_ARG_INDEX]))) {
            invert_my_bmp(&bmp);
            response = write_my_bmp(&bmp, argv[CONVERTER_OUTPUT_FILE_ARG_INDEX]);
            destroy_my_bmp(&bmp);
        }
        switch (response) {
            case SUCCESS:
                return 0;
            case INVALID_VALUE:
                return CONVERTER_INVALID_FILE_STRUCTURE_EXIT_CODE;
            default:
                return CONVERTER_GENERAL_ERROR_EXIT_CODE;
        }
    } else if (strcmp(argv[CONVERTER_IMPLEMENTATION_TYPE_ARG_INDEX], "--theirs") == 0) {
        BMP *bmp = BMP_ReadFile(argv[CONVERTER_INPUT_FILE_ARG_INDEX]);
        BMP_CHECK_ERROR(stderr, CONVERTER_QDBMP_ERROR_EXIT_CODE)
        invert_qdbmp(bmp);
        BMP_CHECK_ERROR(stderr, (BMP_Free(bmp), CONVERTER_QDBMP_ERROR_EXIT_CODE))
        BMP_WriteFile(bmp, argv[CONVERTER_OUTPUT_FILE_ARG_INDEX]);
        BMP_CHECK_ERROR(stderr, (BMP_Free(bmp), CONVERTER_QDBMP_ERROR_EXIT_CODE))
        BMP_Free(bmp);
        return 0;
    } else {
        log_error("Invalid implementation type. Expected \"--mine\" or \"--theirs\", but found \"%s\"\n",
                  argv[CONVERTER_IMPLEMENTATION_TYPE_ARG_INDEX]);
        return CONVERTER_INVALID_COMMAND_LINE_ARGS_EXIT_CODE;
    }
}
