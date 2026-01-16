#include "game_entities.h"
#include "../config.h"
#include "../systems/map.h"
#include "../tic80.h"
#include "../entities/player.h"
#include <stdlib.h>


// 全局实体数组
static GameEntity entities[MAX_GAME_ENTITIES];
static int entity_count = 0;

void game_entities_init() {
    entity_count = 0;
    // 清空实体数组
    for (int i = 0; i < MAX_GAME_ENTITIES; i++) {
        entities[i].type = ENTITY_TYPE_PLAYER;
        entities[i].x = 0;
        entities[i].y = 0;
        entities[i].z = 0;
        entities[i].tile_id = 0;
    }
}

void game_entity_add(EntityType type, float x, float y, float z, int tile_id) {
    if (entity_count >= MAX_GAME_ENTITIES) return;
    
    entities[entity_count].type = type;
    entities[entity_count].x = x;
    entities[entity_count].y = y;
    entities[entity_count].z = z;
    entities[entity_count].tile_id = tile_id;
    entity_count++;
}

GameEntity* game_entities_get_array() {
    return entities;
}

int game_entities_get_count() {
    return entity_count;
}

void game_entities_clear_walls_around() {
    // 为每个实体清除周围 3x3 范围内的墙体
    for (int i = 0; i < entity_count; i++) {
        int center_x = (int)entities[i].x;
        int center_y = (int)entities[i].y;
        
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int tx = center_x + dx;
                int ty = center_y + dy;
                if (tx >= 0 && tx < MAP_SIZE && ty >= 0 && ty < MAP_SIZE) {
                    mset(tx, ty, ID_GRASS);
                }
            }
        }
    }
}

void game_entities_sync_all() {
    // 同步所有实体位置到各自的引用
    for (int i = 0; i < entity_count; i++) {
        if (entities[i].type == ENTITY_TYPE_PLAYER) {
            // 同步玩家位置
            player.pos.x = entities[i].x;
            player.pos.y = entities[i].y;
            player.z = entities[i].z;
        }
        // 未来可以在这里添加其他实体的同步逻辑
        // 例如：
        // else if (entities[i].type == ENTITY_TYPE_ENEMY) {
        //     enemies[enemy_index].pos.x = entities[i].x;
        //     enemies[enemy_index].pos.y = entities[i].y;
        // }
    }
}

void game_entities_update_from_references() {
    // 从引用更新实体位置（例如从player更新到实体系统）
    for (int i = 0; i < entity_count; i++) {
        if (entities[i].type == ENTITY_TYPE_PLAYER) {
            // 从玩家更新实体位置
            entities[i].x = player.pos.x;
            entities[i].y = player.pos.y;
            entities[i].z = player.z;
        }
        // 未来可以在这里添加其他实体的更新逻辑
        // 例如：
        // else if (entities[i].type == ENTITY_TYPE_ENEMY) {
        //     entities[i].x = enemies[enemy_index].pos.x;
        //     entities[i].y = enemies[enemy_index].pos.y;
        // }
    }
}