#include "camera.h"
#include "../tic80.h"
#include "../config.h"
#include "../utils/math_iso.h"
#include <math.h>

float cam_x = 0, cam_y = 0;
float cam_speed_x = 0.15f;
float cam_speed_y = 0.12f;

// 相机漫游状态
static CameraState camera_state = CAMERA_STATE_TOUR;
static CameraWaypoint waypoints[4];
static int current_waypoint = 0;
static Vec2 tour_pos = {32.0f, 32.0f}; // 当前漫游位置
static float tour_speed = 0.15f; // 漫游速度

void camera_tour_init() {
    // 初始化路径点（等轴坐标系边界点）
    // 路径点稍微向内移动，避免相机超出边界
    // (5,5) -> (59,5) -> (59,59) -> (5,59)
    waypoints[0].world_pos = (Vec2){5.0f, 5.0f};
    waypoints[0].speed = tour_speed;

    waypoints[1].world_pos = (Vec2){59.0f, 5.0f};
    waypoints[1].speed = tour_speed;

    waypoints[2].world_pos = (Vec2){50.0f, 45.0f};
    waypoints[2].speed = tour_speed;

    waypoints[3].world_pos = (Vec2){5.0f, 45.0f};
    waypoints[3].speed = tour_speed;

    // 从第一个点开始
    current_waypoint = 0;
    tour_pos = waypoints[0].world_pos;
    camera_state = CAMERA_STATE_TOUR;
}

void camera_tour_update() {
    if (current_waypoint >= 4) {
        // 漫游完成，切换到游戏状态
        camera_state = CAMERA_STATE_PLAYING;
        return;
    }

    Vec2 target = waypoints[current_waypoint].world_pos;
    float speed = waypoints[current_waypoint].speed;

    // 计算方向向量
    Vec2 dir;
    dir.x = target.x - tour_pos.x;
    dir.y = target.y - tour_pos.y;
    float distance = sqrtf(dir.x * dir.x + dir.y * dir.y);

    if (distance < speed) {
        // 到达当前路径点
        tour_pos = target;
        current_waypoint++;

        if (current_waypoint >= 4) {
            // 完成所有路径点
            camera_state = CAMERA_STATE_PLAYING;
            return;
        }
    } else {
        // 向目标点移动
        dir.x /= distance;
        dir.y /= distance;
        tour_pos.x += dir.x * speed;
        tour_pos.y += dir.y * speed;
    }

    // 计算相机位置，使tour_pos位于屏幕中央
    float target_cam_x, target_cam_y;
    get_camera_pos_for_world(tour_pos.x, tour_pos.y, 0.0f, &target_cam_x, &target_cam_y);

    // 直接设置相机位置，避免平滑移动导致的越界问题
    cam_x = target_cam_x;
    cam_y = target_cam_y;
}

int camera_is_touring() {
    return camera_state == CAMERA_STATE_TOUR;
}

void camera_update(Entity* player) {
    if (camera_state == CAMERA_STATE_TOUR) {
        camera_tour_update();
        return;
    }

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
