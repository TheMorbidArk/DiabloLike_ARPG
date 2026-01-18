#include "scene.h"
#include "scene_manager.h"
#include "scene_registry.h"
#include "collision_system.h"
#include "../battle/battle.h"
#include "../player/player.h"
#include "../../core/config.h"
#include "../../tic80.h"
#include "../../core/entity/entity_manager.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 为了保持向后兼容的全局变量（临时）
static int battle_cooldown = 0; // 战斗冷却计时器，防止连续触发

// 初始化场景系统
void scene_init() {
    // 初始化各个子系统
    collision_system_init();
    scene_manager_init();
    scene_registry_init();

    // 自动注册场景
    scene_registry_auto_register();
}

// 切换场景
void scene_switch(GameScene new_scene) {
    scene_manager_switch_to(new_scene);
}

// 获取当前场景
GameScene scene_get_current() {
    return scene_manager_get_current();
}

// 场景更新 - 使用新的架构
void scene_update() {
    if (scene_manager_is_transitioning()) return;

    // 获取当前场景并调用其update方法
    Scene* current_scene = scene_registry_get(scene_manager_get_current());
    if (current_scene && current_scene->vtable && current_scene->vtable->update) {
        SceneContext* ctx = scene_manager_get_context();
        current_scene->vtable->update(ctx);
    }
}

// 场景渲染 - 使用新的架构
void scene_render() {
    if (scene_manager_is_transitioning()) return;

    // 获取当前场景并调用其render方法
    Scene* current_scene = scene_registry_get(scene_manager_get_current());
    if (current_scene && current_scene->vtable && current_scene->vtable->render) {
        SceneContext* ctx = scene_manager_get_context();
        current_scene->vtable->render(ctx);
    }
}

// 以下是向后兼容的接口，委托给新的系统

// 检查与指定类型实体的碰撞
int scene_check_entity_collision(EntityType type) {
    return collision_check_entity_type(type);
}

// 检查与任意实体的碰撞
int scene_check_any_entity_collision() {
    return collision_check_any_entity();
}

// 触发战斗
void scene_trigger_battle(int enemy_type) {
    // 使用新的场景系统来处理战斗触发
    Scene* exploration_scene = scene_registry_get(SCENE_EXPLORATION);
    if (exploration_scene && exploration_scene->context) {
        // 这里可以调用探索场景的战斗触发方法
        // 暂时使用原来的逻辑作为过渡
        SceneContext* ctx = scene_manager_get_context();

        // 保存当前场景数据
        EntityID player_id = entity_get_player();
        entity_get_position(player_id, &ctx->player_x, &ctx->player_y, (float*)NULL);
        ctx->from_scene = SCENE_EXPLORATION;

        // Debug：显示切换场景信息
        char scene_str[64];
        sprintf(scene_str, "Switching to BATTLE scene, enemy type: %d", enemy_type);
        trace(scene_str, COLOR_YELLOW);

        // 初始化战斗并切换场景
        scene_manager_switch_to(SCENE_BATTLE);
    }
}

// 从战斗返回
void scene_return_from_battle(BattleResult result) {
    // 使用新的场景系统来处理战斗结束
    Scene* exploration_scene = scene_registry_get(SCENE_EXPLORATION);
    if (exploration_scene && exploration_scene->context) {
        // 这里可以调用探索场景的战斗返回方法
        // 暂时使用原来的逻辑作为过渡
        SceneContext* ctx = scene_manager_get_context();
        ctx->battle_result = (int)result;

        // Debug：显示战斗结果
        char result_str[64];
        sprintf(result_str, "Battle result: %d", (int)result);
        trace(result_str, COLOR_YELLOW);

        // 安全位置移动逻辑（从原来的代码复制）
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
        if (result == BATTLE_RESULT_VICTORY) {
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
        battle_cooldown = 120;
    }
}

// 检查是否正在过渡场景
int scene_is_transitioning() {
    return scene_manager_is_transitioning();
}

// 获取场景数据
SceneData* scene_get_data() {
    return scene_manager_get_context_data();
}

// 设置场景数据
void scene_set_data(SceneData data) {
    scene_manager_set_context_data(data);
}