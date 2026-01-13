#include "math_iso.h"
#include "../config.h"
#include "../tic80.h"

void world_to_screen(float wx, float wy, float wz, int* sx, int* sy) {
    extern float cam_x, cam_y; // 引用全局摄像机
    *sx = (int)((wx - wy) * (ISO_W / 2)) + (WIDTH / 2);
    *sy = (int)((wx + wy) * (ISO_H / 2));
    *sy -= (int)(wz * 16.0f); 
    *sx -= (int)cam_x;
    *sy -= (int)cam_y;
    *sx -= 8; // Sprite Center Offset
    *sy -= 4; 
}