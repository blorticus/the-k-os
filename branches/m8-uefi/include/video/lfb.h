#ifndef _VIDEO_LFB_
#define _VIDEO_LFB_

#include <sys/types.h>

typedef enum {
    PCS_Unknown                     = 0,
    PCS_32BitReservedRedGreenBlue   = 1
} pixel_color_style;


typedef struct {
    u32*                lfb_base_addr;
    u32                 hpixels;
    u32                 vpixels;
    pixel_color_style   pc_style;
} frame_buffer;


typedef enum {
    LFBE_NoError                    = 0,
    LFBE_PixelOffsetError           = 1
} lfb_error;


/**
 * DESCRIPTION:
 *      Initialize a video linear frame buffer scanning from upper left to lower right,
 *      with the upper-left as the origin (0,0).
 * ARGS:
 *      - fb                : the frame_buffer reference
 *      - buffer_start_addr : the start address of the linear frame buffer (the origin at fb[0])
 *      - hpixels           : the number of horizontal pixels
 *      - vpixels           : the number of vertical pixels
 *      - style             : the pixel coloration style
 * RETURNS:
 *      lfb_error indicating success (0) or failure
 **/
lfb_error lfb_init( frame_buffer* fb, u32* buffer_start_addr, u32 hpixels, u32 vpixels, pixel_color_style style );


/**
 * DESCRIPTION:
 *      Given a matrix of values, if the value at each position is 0 draw the bg_color; otherwise, draw the fg_color.
 *      Matrix position bitmap[0][0] corresponds to the pixel at (at_x, at_y) in frame_buffer fb.
 * ARGS:
 *      - fb            : frame_buffer reference
 *      - at_x          : x origin of bitmap
 *      - at_y          : y origin of bitmap
 *      - bitmap        : matrix of 0 or non-zero corresponding to bg_color or fg_color at each location
 *      - bm_width      : width of bitmap
 *      - bm_height     : height of bitmap
 *      - bg_color      : color of background using the frame_buffer fb pixel_color_style
 *      - fg_color      : color of foreground using the frame_buffer fb pixel_color_style
 * RETURNS:
 *      lfb_error indicating success (0) or failure
 **/
lfb_error lfb_draw_boolean_bitmap( frame_buffer* fb, u32 at_x, u32 at_y, u8* bitmap, u32 bm_width, u32 bm_height, u32 bg_color, u32 fg_color );


/**
 * DESCRIPTION:
 *      Fill the entire background with bg_color appropriate for frame_Buffer fb pixel_Color_style
 * ARGS:
 *      - fb            : frame_buffer reference
 *      - bg_color      : color of background using the frame_buffer fb pixel_color_style
 * RETURNS:
 *      lfb_error indicating success (0) or failure
 **/
lfb_error lfb_fill_background( frame_buffer* fb, u32 bg_color );


/**
 * DESCRIPTION:
 *      Set contiguous pixels to same color
 * ARGS:
 *      - fb            : frame_buffer reference
 *      - first_pixel   : first pixel (offset from fb base) to color
 *      - pixel_count   : number of pixels to color
 *      - fill_color    : color to use for filling rectangle
 * RETURNS:
 *      lfb_error indicating success (0) or failure
 **/
lfb_error lfb_color_pixels( frame_buffer* fb, u32 first_pixel, u32 pixel_count, u32 fill_color );


/**
 * DESCRIPTION:
 *      Move pixel block back by shift offset
 * ARGS:
 *      - fb            : frame_buffer reference
 *      - first_pixel   : First pixel to move
 *      - last_pixel    : Last pixel to move
 *      - shift_start   : Move all pixels between first_pixel through last_pixel backwards to location starting at shift_start pixel
 * RETURNS:
 *      lfb_error indicating success (0) or failure
 * NOTES:
 *      Results are undefined (and potentially dangerous) unless shift_start < first_pixel < last_pixel < fb->width
 **/
lfb_error lfb_shift_up( frame_buffer* fb, u32 first_pixel, u32 last_pixel, u32 shift_start );


#endif
