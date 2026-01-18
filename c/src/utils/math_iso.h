#ifndef MATH_ISO_H
#define MATH_ISO_H
#include "../core/types.h"
void world_to_screen(float wx, float wy, float wz, int* sx, int* sy);
void world_to_screen_pos(float wx, float wy, int* sx, int* sy);
void get_camera_pos_for_world(float wx, float wy, float wz, float* cam_x, float* cam_y);
#endif