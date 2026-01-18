#ifndef BATTLE_SCENE_H
#define BATTLE_SCENE_H

#include "../../scene_interface.h"

// 战斗场景私有数据
typedef struct {
    int is_battle_active;
    int last_battle_result;
} BattleSceneData;

// 获取战斗场景的虚函数表
SceneVTable* battle_scene_get_vtable(void);

// 初始化战斗场景
void battle_scene_init(SceneContext* ctx);

// 进入战斗场景
void battle_scene_enter(SceneContext* ctx);

// 更新战斗场景
void battle_scene_update(SceneContext* ctx);

// 渲染战斗场景
void battle_scene_render(SceneContext* ctx);

// 退出战斗场景
void battle_scene_exit(SceneContext* ctx);

// 清理战斗场景
void battle_scene_cleanup(SceneContext* ctx);

// 初始化新战斗（从外部调用）
void battle_scene_start_battle(SceneContext* ctx, int enemy_type);

// 处理战斗结束
void battle_scene_handle_battle_end(SceneContext* ctx);

#endif