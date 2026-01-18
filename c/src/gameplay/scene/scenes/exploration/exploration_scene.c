#include "exploration_scene.h"
#include "../../collision_system.h"
#include "../../scene_manager.h"
#include "../../scene.h"
#include "../../../../gameplay/battle/battle.h"
#include "../../../../core/entity/entity_manager.h"
#include "../../../../gameplay/player/player.h"
#include "../../../../tic80.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

// 探索场景的虚函数表
static SceneVTable exploration_vtable;

// 探索场景私有数据
static ExplorationSceneData exploration_data;

void exploration_scene_init(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    memset(&exploration_data, 0, sizeof(ExplorationSceneData));
    exploration_data.battle_cooldown = 0;
}

void exploration_scene_enter(SceneContext* ctx) {
    // 从上下文中获取进入探索场景的信息
    // ctx->battle_result 包含上一次战斗的结果（如果是从战斗场景返回）
    // ctx->from_scene 包含从哪个场景进入的

    // 如果是从战斗场景返回，重置战斗冷却时间
    if (ctx->from_scene == SCENE_BATTLE) {
        exploration_data.battle_cooldown = 120; // 设置战斗冷却
    } else {
        exploration_data.battle_cooldown = 0;
    }

    // 可以在这里处理战斗结果
    // 例如：根据战斗结果给予奖励、改变游戏状态等
}

void exploration_scene_update(SceneContext* ctx) {
    // 更新战斗冷却
    if (exploration_data.battle_cooldown > 0) {
        exploration_data.battle_cooldown--;
    }

    // 只有冷却结束后才检查碰撞
    if (exploration_data.battle_cooldown == 0) {
        int collision_index = collision_check_any_entity();
        if (collision_index >= 0) {
            EntityData* entities = entity_get_array();
            EntityType type = entities[collision_index].type;

            if (type == ENTITY_TYPE_RED_BALL) {
                // 碰到红球，触发战斗
                exploration_scene_trigger_battle(ctx, 0); // 0 = Goblin
            }
            // 未来可以添加其他触发器
        }
    }
}

void exploration_scene_render(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    // 探索场景的渲染在renderer.c中处理，这里不需要额外渲染
}

void exploration_scene_exit(SceneContext* ctx) {
    // 退出探索场景时，保存当前玩家位置到上下文中
    // 这样其他场景（如战斗场景）可以知道玩家位置
    EntityID player_id = entity_get_player();
    entity_get_position(player_id, &ctx->player_x, &ctx->player_y, (float*)NULL);

    // 退出探索场景时的清理
}

void exploration_scene_cleanup(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    // 资源清理
}

void exploration_scene_trigger_battle(SceneContext* ctx, int enemy_type) {
    // 保存当前场景数据
    EntityID player_id = entity_get_player();
    entity_get_position(player_id, &ctx->player_x, &ctx->player_y, (float*)NULL);
    ctx->from_scene = SCENE_EXPLORATION;

    // Debug：显示切换场景信息
    char scene_str[64];
    sprintf(scene_str, "Switching to BATTLE scene, enemy type: %d", enemy_type);
    trace(scene_str, COLOR_YELLOW);

    // 初始化战斗
    battle_init(enemy_type);

    // 切换到战斗场景
    scene_manager_switch_to(SCENE_BATTLE);
}

void exploration_scene_return_from_battle(SceneContext* ctx, int battle_result) {
    ctx->battle_result = battle_result;

    // Debug：显示战斗结果
    char result_str[64];
    sprintf(result_str, "Battle result: %d", battle_result);
    trace(result_str, COLOR_YELLOW);

    // 无论战斗结果如何，都将玩家移到安全位置
    EntityData* entities = entity_get_array();
    int entity_count = entity_get_count();
    float red_ball_x = -1.0f, red_ball_y = -1.0f;

    // 找到红球位置
    for (int i = 0; i < entity_count; i++) {
        if (entities[i].type == ENTITY_TYPE_RED_BALL) {
            red_ball_x = entities[i].entity.pos.x;
            red_ball_y = entities[i].entity.pos.y;
            break;
        }
    }

    float new_player_x, new_player_y;
    if (red_ball_x >= 0 && red_ball_y >= 0) {
        // 计算从红球到玩家的方向，将玩家移到红球对面
        float dx = ctx->player_x - red_ball_x;
        float dy = ctx->player_y - red_ball_y;

        // 归一化方向
        float len = sqrtf(dx * dx + dy * dy);
        if (len > 0.1f) {
            dx /= len;
            dy /= len;

            // 移动到红球对面3-4个单位距离（足够安全）
            float distance = 3.5f;
            new_player_x = red_ball_x - dx * distance;
            new_player_y = red_ball_y - dy * distance;
        } else {
            // 如果玩家已经在红球位置，随机移动
            new_player_x = red_ball_x + (rand() % 3 - 1) * 4.0f;
            new_player_y = red_ball_y + (rand() % 3 - 1) * 4.0f;
        }

        // Debug：显示玩家新位置
        char pos_str[128];
        sprintf(pos_str, "Player moved to safe pos: (%.2f, %.2f)", new_player_x, new_player_y);
        trace(pos_str, COLOR_LIGHT_BLUE);
    } else {
        // 找不到红球，保持原位置
        new_player_x = ctx->player_x;
        new_player_y = ctx->player_y;

        // Debug：显示未找到红球
        char no_ball_str[64];
        sprintf(no_ball_str, "Red ball not found, player kept at: (%.2f, %.2f)", new_player_x, new_player_y);
        trace(no_ball_str, COLOR_ORANGE);
    }

    // 更新玩家位置到实体管理器
    EntityID player_id = entity_get_player();
    entity_set_position(player_id, new_player_x, new_player_y, 0.0f);

    // 同步到player全局变量（临时保持兼容性）
    player.pos.x = new_player_x;
    player.pos.y = new_player_y;

    // 根据战斗结果处理
    if (battle_result == BATTLE_RESULT_VICTORY) {
        // 移除触发战斗的实体（红球）
        for (int i = 0; i < entity_count; i++) {
            if (entities[i].type == ENTITY_TYPE_RED_BALL) {
                // 将实体移到地图外，相当于删除
                entity_remove(i);

                // Debug：显示红球已删除
                trace("Red ball removed after victory", COLOR_GREEN);
                break;
            }
        }
    }

    // 返回探索场景
    scene_manager_switch_to(SCENE_EXPLORATION);

    // 设置更长的战斗冷却，防止立即再次触发（120帧=2秒）
    exploration_data.battle_cooldown = 120;
}

SceneVTable* exploration_scene_get_vtable(void) {
    exploration_vtable.init = exploration_scene_init;
    exploration_vtable.enter = exploration_scene_enter;
    exploration_vtable.update = exploration_scene_update;
    exploration_vtable.render = exploration_scene_render;
    exploration_vtable.exit = exploration_scene_exit;
    exploration_vtable.cleanup = exploration_scene_cleanup;

    return &exploration_vtable;
}