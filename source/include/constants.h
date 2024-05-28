#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

extern const int
    SCREEN_WIDTH, // 1280,
    SCREEN_HEIGHT, // 720,
    SCREEN_HW,
    SCREEN_HH,
    SCREEN_FPS;
extern float
    CAMERA_fow_horizontal,
    CAMERA_sight_screen_width,
    CAMERA_fow_vertical,
    CAMERA_sight_screen_height;
extern const float
    cube_side,
    ch,
    depth_start,
    depth_mid,
    scale_base,
    universal_circle_base;
extern const uint8_t
    box_amount,
    vertex_inside_one_box,
    total_v;

#endif
