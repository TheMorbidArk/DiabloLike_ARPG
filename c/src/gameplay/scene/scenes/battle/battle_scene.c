#include "battle_scene.h"
#include "../../scene_manager.h"
#include "../../../battle/battle.h"
#include "../exploration/exploration_scene.h"
#include <string.h>

// 战斗场景的虚函数表
static SceneVTable battle_vtable;

// 战斗场景私有数据
static BattleSceneData battle_data;

void battle_scene_init(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    memset(&battle_data, 0, sizeof(BattleSceneData));
}

void battle_scene_enter(SceneContext* ctx) {
    // 从上下文中获取进入战斗场景时的信息
    // ctx->player_x, ctx->player_y 包含玩家位置
    // ctx->from_scene 包含从哪个场景进入的

    // 验证是从探索场景进入的（正常情况）
    if (ctx->from_scene == SCENE_EXPLORATION) {
        // 可以在这里记录战斗开始时的玩家位置
        // 例如用于战斗后的位置恢复或战斗统计
    }

    // 战斗场景进入时的处理
    battle_data.is_battle_active = 1;
    battle_data.last_battle_result = 0;
}

void battle_scene_update(SceneContext* ctx) {
    if (!battle_data.is_battle_active) return;

    // 更新战斗逻辑
    battle_update();

    // 检查战斗是否结束
    if (battle_is_over()) {
        BattleResult result = battle_get_result();
        battle_data.last_battle_result = (int)result;
        battle_scene_handle_battle_end(ctx);
    }
}

void battle_scene_render(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    if (battle_data.is_battle_active) {
        battle_render();
    }
}

void battle_scene_exit(SceneContext* ctx) {
    // 退出战斗场景时，将战斗结果保存到上下文中
    ctx->battle_result = battle_data.last_battle_result;

    // 退出战斗场景时的处理
    battle_data.is_battle_active = 0;

    // 清理战斗资源
    battle_cleanup();
}

void battle_scene_cleanup(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    // 清理战斗场景资源
    battle_cleanup();
}

void battle_scene_start_battle(SceneContext* ctx, int enemy_type) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    // 初始化战斗
    battle_init(enemy_type);
    battle_data.is_battle_active = 1;
}

void battle_scene_handle_battle_end(SceneContext* ctx) {
    // 处理战斗结束，返回探索场景
    exploration_scene_return_from_battle(ctx, battle_data.last_battle_result);
}

SceneVTable* battle_scene_get_vtable(void) {
    battle_vtable.init = battle_scene_init;
    battle_vtable.enter = battle_scene_enter;
    battle_vtable.update = battle_scene_update;
    battle_vtable.render = battle_scene_render;
    battle_vtable.exit = battle_scene_exit;
    battle_vtable.cleanup = battle_scene_cleanup;

    return &battle_vtable;
}