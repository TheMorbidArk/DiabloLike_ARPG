#include "camera.h"
#include "../tic80.h"
#include "../config.h"
#include <math.h>

float cam_x = 0, cam_y = 0;
float cam_speed_x = 0.15f; 
float cam_speed_y = 0.12f; 

void camera_update(Entity* player) {
    float target_iso_x = (player->pos.x - player->pos.y) * (ISO_W / 2.0f);
    float target_iso_y = (player->pos.x + player->pos.y) * (ISO_H / 2.0f);
    target_iso_y -= (player->z * 16.0f);
    target_iso_x += (WIDTH / 2.0f);
    float abs_target_x = target_iso_x - 8.0f;
    float abs_target_y = target_iso_y - 4.0f;
    float ideal_cam_x = abs_target_x - (WIDTH / 2.0f);
    float ideal_cam_y = abs_target_y - (HEIGHT / 2.0f);
    cam_x += (ideal_cam_x - cam_x) * cam_speed_x;
    cam_y += (ideal_cam_y - cam_y) * cam_speed_y;
}