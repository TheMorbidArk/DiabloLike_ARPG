#include "collision_system.h"
#include "../../core/entity/entity_manager.h"
#include "../../core/config.h"
#include "../../tic80.h"
#include <math.h>
#include <stdio.h>

// 内部函数：计算两点间的距离
static float calculate_distance(float x1, float y1, float x2, float y2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return sqrtf(dx * dx + dy * dy);
}

void collision_system_init(void) {
    // 碰撞系统初始化（目前无需特殊初始化）
}

int collision_check_entity_type(EntityType type) {
    EntityData* entities = entity_get_array();
    int entity_count = entity_get_count();

    float player_x, player_y, player_z;
    EntityID player_id = entity_get_player();
    entity_get_position(player_id, &player_x, &player_y, &player_z);

    for (int i = 0; i < entity_count; i++) {
        if (entities[i].type == type) {
            float entity_x = entities[i].entity.pos.x;
            float entity_y = entities[i].entity.pos.y;
            float entity_z = entities[i].entity.z;

            // 碰撞检测：检查玩家是否在实体周围范围内
            if (fabsf(player_x - entity_x) <= 1.5f &&
                fabsf(player_y - entity_y) <= 1.5f &&
                fabsf(player_z - entity_z) < COLLISION_Z_THRESHOLD) {
                return i;
            }
        }
    }

    return -1;
}

int collision_check_any_entity(void) {
    EntityData* entities = entity_get_array();
    int entity_count = entity_get_count();

    float player_x, player_y, player_z;
    EntityID player_id = entity_get_player();
    entity_get_position(player_id, &player_x, &player_y, &player_z);

    for (int i = 0; i < entity_count; i++) {
        // 跳过玩家实体自己
        if (entities[i].type == ENTITY_TYPE_PLAYER) {
            continue;
        }

        float entity_x = entities[i].entity.pos.x;
        float entity_y = entities[i].entity.pos.y;
        float entity_z = entities[i].entity.z;

        // 调试信息输出（距离小于5时）
        if (calculate_distance(player_x, player_y, entity_x, entity_y) < 5.0f) {
            char debug_str[128];
            const char* type_name = (entities[i].type == ENTITY_TYPE_RED_BALL) ? "RED_BALL" : "OTHER";
            sprintf(debug_str, "Player(%.4f,%.4f,%.4f) vs %s[%d](%.4f,%.4f,%.4f) dist(%.4f,%.4f,%.4f)",
                    player_x, player_y, player_z, type_name, i, entity_x, entity_y, entity_z,
                    player_x - entity_x, player_y - entity_y, player_z - entity_z);
            trace(debug_str, COLOR_YELLOW);
        }

        // 碰撞检测
        if (fabsf(player_x - entity_x) <= 1.5f &&
            fabsf(player_y - entity_y) <= 1.5f &&
            fabsf(player_z - entity_z) < COLLISION_Z_THRESHOLD) {
            return i;
        }
    }

    return -1;
}

CollisionResult collision_get_detailed_info(int entity_index) {
    CollisionResult result = {-1, 0.0f, ENTITY_TYPE_PLAYER};

    if (entity_index < 0) return result;

    EntityData* entities = entity_get_array();
    int entity_count = entity_get_count();

    if (entity_index >= entity_count) return result;

    float player_x, player_y, player_z;
    EntityID player_id = entity_get_player();
    entity_get_position(player_id, &player_x, &player_y, &player_z);

    EntityData* entity = &entities[entity_index];
    float entity_x = entity->entity.pos.x;
    float entity_y = entity->entity.pos.y;

    result.entity_index = entity_index;
    result.distance = calculate_distance(player_x, player_y, entity_x, entity_y);
    result.entity_type = entity->type;

    return result;
}

int collision_check_in_range(float max_distance) {
    EntityData* entities = entity_get_array();
    int entity_count = entity_get_count();

    float player_x, player_y, player_z;
    EntityID player_id = entity_get_player();
    entity_get_position(player_id, &player_x, &player_y, &player_z);

    for (int i = 0; i < entity_count; i++) {
        if (entities[i].type == ENTITY_TYPE_PLAYER) continue;

        float entity_x = entities[i].entity.pos.x;
        float entity_y = entities[i].entity.pos.y;

        if (calculate_distance(player_x, player_y, entity_x, entity_y) <= max_distance) {
            return i;
        }
    }

    return -1;
}

void collision_debug_info(int entity_index) {
    if (entity_index < 0) return;

    CollisionResult info = collision_get_detailed_info(entity_index);
    if (info.entity_index >= 0) {
        char debug_str[64];
        sprintf(debug_str, "Collision: Entity %d (Type:%d) Distance:%.2f",
                info.entity_index, info.entity_type, info.distance);
        trace(debug_str, COLOR_CYAN);
    }
}