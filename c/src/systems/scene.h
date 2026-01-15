#ifndef SCENE_H
#define SCENE_H

#include "../config.h"
#include "../entities/player.h"
#include "../entities_system/game_entities.h"
#include "battle.h"

// 场景数据结构 - 用于场景间传递数据
typedef struct {
    float player_x;
    float player_y;
    int from_scene;
    int battle_result;
} SceneData;

// 场景管理函数

// 初始化场景系统
void scene_init();

// 切换到指定场景
void scene_switch(GameScene new_scene);

// 获取当前场景
GameScene scene_get_current();

// 场景更新（每帧调用，内部根据当前场景调用相应的更新函数）
void scene_update();

// 场景渲染（内部根据当前场景调用相应的渲染函数）
void scene_render();

// 检查玩家是否与指定类型的实体发生碰撞
// 返回碰撞的实体索引，-1表示未碰撞
int scene_check_entity_collision(EntityType type);

// 检查玩家是否与任意实体碰撞
// 返回碰撞的实体索引，-1表示未碰撞
int scene_check_any_entity_collision();

// 触发战斗场景
// enemy_type: 敌人类型（0=Goblin, 1=Orc等）
void scene_trigger_battle(int enemy_type);

// 从战斗返回探索场景
// result: 战斗结果
void scene_return_from_battle(BattleResult result);

// 检查是否正在过渡场景
int scene_is_transitioning();

// 获取场景间传递的数据
SceneData* scene_get_data();

// 设置场景间传递的数据
void scene_set_data(SceneData data);

#endif