#ifndef EXPLORATION_SCENE_H
#define EXPLORATION_SCENE_H

#include "../../scene_interface.h"

// 探索场景私有数据
typedef struct {
    int battle_cooldown;  // 战斗冷却计时器
} ExplorationSceneData;

// 获取探索场景的虚函数表
SceneVTable* exploration_scene_get_vtable(void);

// 初始化探索场景
void exploration_scene_init(SceneContext* ctx);

// 进入探索场景
void exploration_scene_enter(SceneContext* ctx);

// 更新探索场景
void exploration_scene_update(SceneContext* ctx);

// 渲染探索场景
void exploration_scene_render(SceneContext* ctx);

// 退出探索场景
void exploration_scene_exit(SceneContext* ctx);

// 清理探索场景
void exploration_scene_cleanup(SceneContext* ctx);

// 触发战斗（从探索场景调用）
void exploration_scene_trigger_battle(SceneContext* ctx, int enemy_type);

// 从战斗返回探索场景
void exploration_scene_return_from_battle(SceneContext* ctx, int battle_result);

#endif