#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "../../core/entity/entity_manager.h"

// 碰撞检测配置
// 注意：COLLISION_Z_THRESHOLD 在 config.h 中定义，这里不再重复定义

// 碰撞检测结果
typedef struct {
    int entity_index;      // 碰撞的实体索引 (-1表示无碰撞)
    float distance;        // 距离
    EntityType entity_type; // 实体类型
} CollisionResult;

// 初始化碰撞系统
void collision_system_init(void);

// 检查与指定类型实体的碰撞
int collision_check_entity_type(EntityType type);

// 检查与任意实体的碰撞
int collision_check_any_entity(void);

// 获取详细的碰撞信息
CollisionResult collision_get_detailed_info(int entity_index);

// 高级碰撞检测 (带距离限制)
int collision_check_in_range(float max_distance);

// 调试用的碰撞信息输出
void collision_debug_info(int entity_index);

#endif