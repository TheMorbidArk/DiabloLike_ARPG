#include "math_iso.h"
#include "../core/config.h"
#include "../tic80.h"
#include "../rendering/camera/camera.h"

void world_to_screen(float wx, float wy, float wz, int* sx, int* sy) {
    Camera* cam = camera_get_main();
    *sx = (int)((wx - wy) * (ISO_W / 2)) + (WIDTH / 2);
    *sy = (int)((wx + wy) * (ISO_H / 2));
    *sy -= (int)(wz * 16.0f); 
    *sx -= (int)cam->x;
    *sy -= (int)cam->y;
    *sx -= 8; // Sprite Center Offset
    *sy -= 4; 
}

void world_to_screen_pos(float wx, float wy, int* sx, int* sy) {
    *sx = (int)((wx - wy) * (ISO_W / 2));
    *sy = (int)((wx + wy) * (ISO_H / 2));
}

void get_camera_pos_for_world(float wx, float wy, float wz, float* cam_x, float* cam_y) {
    // 将世界坐标转换为等轴屏幕坐标
    float iso_x = (wx - wy) * (ISO_W / 2.0f);
    float iso_y = (wx + wy) * (ISO_H / 2.0f);
    iso_y -= (wz * 16.0f);
    
    // 计算相机位置，使该点位于屏幕中央
    float abs_x = iso_x - 8.0f;
    float abs_y = iso_y - 4.0f;
    *cam_x = abs_x - (WIDTH / 2.0f);
    *cam_y = abs_y - (HEIGHT / 2.0f);
}