#ifndef GAME_ENTITIES_H
#define GAME_ENTITIES_H

#include "../types.h"

// 初始化游戏实体系统
void game_entities_init();

// 添加一个游戏实体
// type: 实体类型
// x, y: 世界坐标
// tile_id: 精灵ID
void game_entity_add(EntityType type, float x, float y, int tile_id);

// 获取游戏实体数组指针
GameEntity* game_entities_get_array();

// 获取游戏实体数量
int game_entities_get_count();

// 在地图生成时清除所有实体周围 3x3 范围内的墙体
void game_entities_clear_walls_around();

// 同步所有实体位置到各自的引用（如player实体）
void game_entities_sync_all();

// 从引用更新实体位置（例如从player更新到实体系统）
void game_entities_update_from_references();

#endif