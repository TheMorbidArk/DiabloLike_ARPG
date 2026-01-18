#ifndef MAP_H
#define MAP_H
#include <stdbool.h>
#include <stdint.h>
#include "../../core/context.h"

void map_init();
bool is_solid(float x, float y, float z);
void map_generate(unsigned int seed, bool generate_walls);
bool map_is_inside(int x, int y);

int map_get_tile(int x, int y);
void map_set_tile(int x, int y, int tile_id);

// 上下文保存/恢复
void map_save_scene(MapScene* state);
void map_restore_scene(const MapScene* state);

#endif