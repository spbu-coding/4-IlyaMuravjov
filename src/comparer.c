#include <memory.h>
#include <assert.h>
#include "my_bmp.h"
#include "logging_std_wrappers.h"

static const int COMPARER_INCOMPARABLE_IMAGES_EXIT_CODE = -1;
static const int COMPARER_GENERAL_ERROR_EXIT_CODE = -2;
static const int COMPARER_INVALID_COMMAND_LINE_ARGS_EXIT_CODE = -4;

static const int DIFFERENCE_ARR_MAX_LENGTH = 100;

static const int COMPARER_FIRST_FILE_ARG_INDEX = 0;
static const int COMPARER_SECOND_FILE_ARG_INDEX = 1;

static const int COMPARER_REQUIRED_ARG_COUNT = 2;

int compare_my_bmp(MY_BMP *bmp1, MY_BMP *bmp2, PIXEL_POS difference_arr[], size_t difference_arr_max_length) {
    assert(bmp1 != NULL);
    assert(bmp2 != NULL);
    assert(difference_arr != NULL);
    if (get_my_bmp_abs_width(bmp1) != get_my_bmp_abs_width(bmp2) || get_my_bmp_abs_height(bmp1) != get_my_bmp_abs_height(bmp2)) {
        log_error("Unable to compare images with different dimensions\n");
        return COMPARER_INCOMPARABLE_IMAGES_EXIT_CODE;
    }
    int difference_count = 0;
    PIXEL_POS_ITERATOR pixel_pos_iterator = get_my_bmp_pixel_pos_iterator(bmp1);
    while (has_next_pixel_pos(&pixel_pos_iterator)) {
        PIXEL_POS pixel_pos = get_next_pixel_pos(&pixel_pos_iterator);
        if (memcmp(get_my_bmp_pixel_color_ptr(bmp1, pixel_pos), get_my_bmp_pixel_color_ptr(bmp2, pixel_pos), sizeof(COLOR)) != 0) {
            if (difference_count < difference_arr_max_length) difference_arr[difference_count] = pixel_pos;
            difference_count++;
        }
    }
    return difference_count;
}

int main(int argc, char *argv[]) {
    argc--, argv++; // ignore the name of the program
    if (argc != COMPARER_REQUIRED_ARG_COUNT) {
        log_error("Invalid number of command line arguments. Expected %d arg(s), but found %d arg(s)\n", COMPARER_REQUIRED_ARG_COUNT, argc);
        return COMPARER_INVALID_COMMAND_LINE_ARGS_EXIT_CODE;
    }
    MY_BMP bmp1, bmp2;
    RESPONSE response;
    int difference_count;
    if (is_success(response = read_my_bmp(&bmp1, argv[COMPARER_FIRST_FILE_ARG_INDEX]))) {
        if (is_success(response = read_my_bmp(&bmp2, argv[COMPARER_SECOND_FILE_ARG_INDEX]))) {
            PIXEL_POS difference_arr[DIFFERENCE_ARR_MAX_LENGTH];
            difference_count = compare_my_bmp(&bmp1, &bmp2, difference_arr, DIFFERENCE_ARR_MAX_LENGTH);
            for (int i = 0; i < difference_count && i < DIFFERENCE_ARR_MAX_LENGTH; i++)
                fprintf(stderr, "(%d, %d)\n", difference_arr[i].x, difference_arr[i].y);
            destroy_my_bmp(&bmp2);
        }
        destroy_my_bmp(&bmp1);
    }
    return is_success(response) ? difference_count : COMPARER_GENERAL_ERROR_EXIT_CODE;
}
