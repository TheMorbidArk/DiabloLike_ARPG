#ifndef CAMERA_H
#define CAMERA_H
#include "../types.h"

typedef enum {
    CAMERA_STATE_TOUR,
    CAMERA_STATE_PLAYING
} CameraState;

typedef struct {
    Vec2 world_pos;
    float speed;
} CameraWaypoint;

void camera_tour_init();
void camera_tour_update();
void camera_update(Entity* p);
int camera_is_touring();

#endif