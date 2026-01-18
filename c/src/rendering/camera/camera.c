#include "camera.h"
#include "../../tic80.h"
#include "../../core/config.h"
#include "../../utils/math_iso.h"
#include <math.h>

static Camera main_camera = {
    .x = 0.0f,
    .y = 0.0f,
    .speed_x = 0.15f,
    .speed_y = 0.12f,
    .state = CAMERA_STATE_TOUR,
    .current_waypoint = 0,
    .tour_pos = {32.0f, 32.0f},
    .tour_speed = 0.25f
};

// 简单的平方根近似函数
static float sqrtf_approx(float x) {
    if (x <= 0.0f) return 0.0f;
    
    // 使用快速平方根近似（牛顿迭代法一次迭代）
    float y = x * 0.5f;
    float z = y;
    
    // 将浮点数位模式解释为整数（用于快速逆平方根）
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1);
    z = *(float*)&i;
    
    // 一次牛顿迭代
    z = z * (1.5f - (x * 0.5f * z * z));
    
    return x * z;
}

void camera_tour_init() {
    // 初始化路径点（等轴坐标系边界点）
    // 路径点稍微向内移动，避免相机超出边界
    // (5,5) -> (59,5) -> (59,59) -> (5,59)
    main_camera.waypoints[0].world_pos = (Vec2){5.0f, 5.0f};
    main_camera.waypoints[0].speed = main_camera.tour_speed;

    main_camera.waypoints[1].world_pos = (Vec2){59.0f, 5.0f};
    main_camera.waypoints[1].speed = main_camera.tour_speed;

    main_camera.waypoints[2].world_pos = (Vec2){50.0f, 45.0f};
    main_camera.waypoints[2].speed = main_camera.tour_speed;

    main_camera.waypoints[3].world_pos = (Vec2){10.0f, 45.0f};
    main_camera.waypoints[3].speed = main_camera.tour_speed;

    // 从第一个点开始
    main_camera.current_waypoint = 0;
    main_camera.tour_pos = main_camera.waypoints[0].world_pos;
    main_camera.state = CAMERA_STATE_TOUR;
}

void camera_tour_update() {
    if (main_camera.current_waypoint >= 4) {
        // 漫游完成，切换到游戏状态
        main_camera.state = CAMERA_STATE_PLAYING;
        return;
    }

    Vec2 target = main_camera.waypoints[main_camera.current_waypoint].world_pos;
    float speed = main_camera.waypoints[main_camera.current_waypoint].speed;

    // 计算方向向量
    Vec2 dir;
    dir.x = target.x - main_camera.tour_pos.x;
    dir.y = target.y - main_camera.tour_pos.y;
    float distance = sqrtf_approx(dir.x * dir.x + dir.y * dir.y);

    if (distance < speed) {
        // 到达当前路径点
        main_camera.tour_pos = target;
        main_camera.current_waypoint++;

        if (main_camera.current_waypoint >= 4) {
            // 完成所有路径点
            main_camera.state = CAMERA_STATE_PLAYING;
            return;
        }
    } else {
        // 向目标点移动
        dir.x /= distance;
        dir.y /= distance;
        main_camera.tour_pos.x += dir.x * speed;
        main_camera.tour_pos.y += dir.y * speed;
    }

    // 计算相机位置，使tour_pos位于屏幕中央
    float target_cam_x, target_cam_y;
    get_camera_pos_for_world(main_camera.tour_pos.x, main_camera.tour_pos.y, 0.0f, &target_cam_x, &target_cam_y);

    // 直接设置相机位置，避免平滑移动导致的越界问题
    main_camera.x = target_cam_x;
    main_camera.y = target_cam_y;
}

int camera_is_touring() {
    return main_camera.state == CAMERA_STATE_TOUR;
}

void camera_update(Entity* player) {
    if (main_camera.state == CAMERA_STATE_TOUR) {
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
    main_camera.x += (ideal_cam_x - main_camera.x) * main_camera.speed_x;
    main_camera.y += (ideal_cam_y - main_camera.y) * main_camera.speed_y;
}

// 上下文保存/恢复
void camera_save_gameplay(CameraGameplay* state) {
    state->x = main_camera.x;
    state->y = main_camera.y;
    state->follow_mode = (int)main_camera.state;  // 简化
}

void camera_restore_gameplay(const CameraGameplay* state) {
    main_camera.x = state->x;
    main_camera.y = state->y;
    main_camera.state = (CameraState)state->follow_mode;
}

Camera* camera_get_main() {
    return &main_camera;
}

void camera_get_position(Camera* cam, float* x, float* y) {
    *x = cam->x;
    *y = cam->y;
}
