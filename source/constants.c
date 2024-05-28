#include <stdint.h>

const int
    SCREEN_WIDTH = 1280, // 1280,
    SCREEN_HEIGHT = 720, // 720,
    SCREEN_HW = SCREEN_WIDTH >> 1,
    SCREEN_HH = SCREEN_HEIGHT >> 1,
    SCREEN_FPS = 144;
float
    CAMERA_fow_horizontal,
    CAMERA_sight_screen_width,
    CAMERA_fow_vertical,
    CAMERA_sight_screen_height;
const float
    cube_side = 1,
    ch = cube_side * 0.5f,
    depth_start = 5,
    depth_mid = depth_start + 1,
    scale_base = 1,
    universal_circle_base = SCREEN_WIDTH >> 2;
const uint8_t
    box_amount = 27,
    vertex_inside_one_box = 9,
    total_v = box_amount * vertex_inside_one_box;
