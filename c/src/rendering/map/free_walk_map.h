#ifndef FREE_WALK_MAP_H
#define FREE_WALK_MAP_H

#include <stdbool.h>
#include <stdint.h>
#include "../../core/entity/entity_manager.h"

// 自由行走地图尺寸
#define FREE_WALK_MAP_WIDTH 16
#define FREE_WALK_MAP_HEIGHT 96
#define FREE_WALK_MAP_SIZE (FREE_WALK_MAP_WIDTH * FREE_WALK_MAP_HEIGHT)

// 自由行走地图结构体
typedef struct {
    uint8_t tiles[FREE_WALK_MAP_SIZE];  // 固定大小数组，存储tile ID
} FreeWalkMap;

// 初始化地图
void free_walk_map_init(FreeWalkMap* map);

// 生成地图：边界墙，内部草地
void free_walk_map_generate(FreeWalkMap* map);

// 获取tile
int free_walk_map_get_tile(const FreeWalkMap* map, int x, int y);

// 设置tile
void free_walk_map_set_tile(FreeWalkMap* map, int x, int y, int tile_id);

// 检查是否为固体（墙体）
bool free_walk_map_is_solid(const FreeWalkMap* map, float x, float y, float z);

// 渲染地图（与renderer集成）
void free_walk_map_render(const FreeWalkMap* map, Entity* player);

#endif