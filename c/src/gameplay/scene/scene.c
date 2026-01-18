#include "scene.h"
#include "../battle/battle.h"
#include "../player/player.h"
#include "../../core/config.h"
#include "../../tic80.h"
#include "../../core/entity/entity_manager.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 当前场景状态
static GameScene current_scene = SCENE_EXPLORATION;
static GameScene previous_scene = SCENE_EXPLORATION;
static int is_transitioning = 0;
static SceneData scene_data = {0};
static int battle_cooldown = 0; // 战斗冷却计时器，防止连续触发

// 场景初始化
void scene_init() {
    current_scene = SCENE_EXPLORATION;
    previous_scene = SCENE_EXPLORATION;
    is_transitioning = 0;
    scene_data.player_x = 0.0f; scene_data.player_y = 0.0f; scene_data.from_scene = 0; scene_data.battle_result = 0;
}

// 切换场景
void scene_switch(GameScene new_scene) {
    if (current_scene == new_scene) return;

    is_transitioning = 1;

    // 保存当前场景的数据
    scene_data.from_scene = (int)current_scene;

    // 如果是进入战斗场景，保存玩家位置
    if (new_scene == SCENE_BATTLE && current_scene == SCENE_EXPLORATION) {
        EntityID player_id = entity_get_player();
        entity_get_position(player_id, &scene_data.player_x, &scene_data.player_y, (float*)0);
    }

    // 退出当前场景
    if (current_scene == SCENE_BATTLE) {
        battle_cleanup();
    }

    // 切换场景
    previous_scene = current_scene;
    current_scene = new_scene;

    // 进入新场景
    if (new_scene == SCENE_BATTLE) {
        // 战斗场景会在触发时初始化
    }

    is_transitioning = 0;
}

// 获取当前场景
GameScene scene_get_current() {
    return current_scene;
}

// 场景更新
void scene_update() {
    if (is_transitioning) return;

    switch (current_scene) {
        case SCENE_EXPLORATION:
            // 探索场景：检查碰撞，可能触发战斗
            {
                // 更新战斗冷却
                if (battle_cooldown > 0) {
                    battle_cooldown--;
                }

                // 只有冷却结束后才检查碰撞
                if (battle_cooldown == 0) {
                    int collision_index = scene_check_any_entity_collision();
                    if (collision_index >= 0) {
                        EntityData* entities = entity_get_array();
                        EntityType type = entities[collision_index].type;

                        if (type == ENTITY_TYPE_RED_BALL) {
                            // Debug：战斗触发信息
//                            char trigger_str[64];
//                            sprintf(trigger_str, "Battle triggered! Entity at (%.2f, %.2f)",
//                                    entities[collision_index].x, entities[collision_index].y);
//                            trace(trigger_str, COLOR_RED);

                            // 碰到红球，触发战斗
                            scene_trigger_battle(0); // 0 = Goblin
                        }
                        // 未来可以添加其他触发器
                    }
                }
            }
            break;

        case SCENE_BATTLE:
            battle_update();
            if (battle_is_over()) {
                BattleResult result = battle_get_result();
                scene_return_from_battle(result);
            }
            break;

        case SCENE_DIALOG:
            // 对话场景（暂未实现）
            break;

        case SCENE_MENU:
            // 菜单场景（暂未实现）
            break;

        default:
            break;
    }
}

// 场景渲染
void scene_render() {
    if (is_transitioning) return;

    switch (current_scene) {
        case SCENE_EXPLORATION:
            // 探索场景的渲染在renderer.c中处理
            break;

        case SCENE_BATTLE:
            battle_render();
            break;

        case SCENE_DIALOG:
            // 对话场景渲染（暂未实现）
            break;

        case SCENE_MENU:
            // 菜单场景渲染（暂未实现）
            break;

        default:
            break;
    }
}

// 检查与指定类型实体的碰撞
int scene_check_entity_collision(EntityType type) {
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

            // 碰撞检测：检查玩家是否在实体周围2x2范围内
            float dx = player_x - entity_x;
            float dy = player_y - entity_y;
            float dz = player_z - entity_z;

            // 只在接近时输出debug信息（距离小于5）
            if ((dx < 0.0f ? -dx : dx) < 5.0f && (dy < 0.0f ? -dy : dy) < 5.0f) {
//                char debug_str[128];
//                sprintf(debug_str, "RED_BALL: Player(%.4f,%.4f,%.4f) vs Entity(%.4f,%.4f,%.4f) dist(%.4f,%.4f,%.4f)",
//                        player_x, player_y, player_z, entity_x, entity_y, entity_z, dx, dy, dz);
//                trace(debug_str, COLOR_WHITE);
            }

            if (fabsf(dx) <= 1.5f && fabsf(dy) <= 1.5f && fabsf(dz) < COLLISION_Z_THRESHOLD) {
                return i;
            }
        }
    }

    return -1;
}

// 检查与任意实体的碰撞
int scene_check_any_entity_collision() {
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

        // 碰撞检测：检查玩家是否在实体周围2x2范围内
        float dx = player_x - entity_x;
        float dy = player_y - entity_y;
        float dz = player_z - entity_z;

        // 只在接近时输出debug信息（距离小于5）
        if (fabsf(dx) < 5.0f && fabsf(dy) < 5.0f) {
            char debug_str[128];
            const char* type_name = (entities[i].type == ENTITY_TYPE_RED_BALL) ? "RED_BALL" : "OTHER";
            sprintf(debug_str, "Player(%.4f,%.4f,%.4f) vs %s[%d](%.4f,%.4f,%.4f) dist(%.4f,%.4f,%.4f)",
                    player_x, player_y, player_z, type_name, i, entity_x, entity_y, entity_z, dx, dy, dz);
            trace(debug_str, COLOR_YELLOW);
        }

        if (fabsf(dx) <= 1.5f && fabsf(dy) <= 1.5f && fabsf(dz) < COLLISION_Z_THRESHOLD) {
            return i;
        }
    }

    return -1;
}

// 触发战斗
void scene_trigger_battle(int enemy_type) {
    // 保存当前场景数据
    EntityID player_id = entity_get_player();
    entity_get_position(player_id, &scene_data.player_x, &scene_data.player_y, (float*)0);
    scene_data.from_scene = SCENE_EXPLORATION;

    // Debug：显示切换场景信息
    char scene_str[64];
    sprintf(scene_str, "Switching to BATTLE scene, enemy type: %d", enemy_type);
    trace(scene_str, COLOR_YELLOW);

    // 初始化战斗
    battle_init(enemy_type);

    // 切换到战斗场景
    scene_switch(SCENE_BATTLE);
}

// 从战斗返回
void scene_return_from_battle(BattleResult result) {
    scene_data.battle_result = (int)result;

    // Debug：显示战斗结果
    char result_str[64];
    sprintf(result_str, "Battle result: %d", (int)result);
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
        float dx = scene_data.player_x - red_ball_x;
        float dy = scene_data.player_y - red_ball_y;

        // 归一化方向
        float len = sqrtf(dx * dx + dy * dy);
        if (len > 0.1f) {
            dx /= len;
            dy /= len;

            // 移动到红球对面3-4个单位距离（足够安全）
            float distance = 3.5f; // 可以是3.0-4.0之间的随机值
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
        new_player_x = scene_data.player_x;
        new_player_y = scene_data.player_y;
        
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
    scene_switch(SCENE_EXPLORATION);
    
    // 设置更长的战斗冷却，防止立即再次触发（120帧=2秒）
    battle_cooldown = 120;
}

// 检查是否正在过渡场景
int scene_is_transitioning() {
    return is_transitioning;
}

// 获取场景数据
SceneData* scene_get_data() {
    return &scene_data;
}

// 设置场景数据
void scene_set_data(SceneData data) {
    scene_data = data;
}
