#ifndef CAMERA_H
#define CAMERA_H
#include "../../core/types.h"

typedef struct {
    Vec2 world_pos;
    float speed;
} CameraWaypoint;

typedef struct {
    float x;
    float y;
    float speed_x;
    float speed_y;
    CameraState state;
    CameraWaypoint waypoints[4];
    int current_waypoint;
    Vec2 tour_pos;
    float tour_speed;
} Camera;

void camera_tour_init();
void camera_tour_update();
void camera_update(Entity* p);
int camera_is_touring();

Camera* camera_get_main();
void camera_get_position(Camera* cam, float* x, float* y);

// 上下文保存/恢复
void camera_save_gameplay(CameraGameplay* state);
void camera_restore_gameplay(const CameraGameplay* state);

#endif