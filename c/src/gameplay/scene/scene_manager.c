#include "scene_manager.h"
#include "scene.h"
#include "scene_registry.h"
#include "../battle/battle.h"
#include "../../core/entity/entity_manager.h"
#include "../../tic80.h"
#include <stdlib.h>
#include <string.h>

// 场景管理器单例实例
static SceneManager scene_manager_instance;

// 场景注册表（扩展架构用）
#define MAX_REGISTERED_SCENES 16
static Scene registered_scenes[MAX_REGISTERED_SCENES];
static int registered_scene_count = 0;

SceneManager* scene_manager_get(void) {
    return &scene_manager_instance;
}

void scene_manager_init(void) {
    SceneManager* mgr = &scene_manager_instance;

    mgr->current_scene = SCENE_EXPLORATION;
    mgr->previous_scene = SCENE_EXPLORATION;
    mgr->is_transitioning = 0;

    // 初始化上下文
    memset(&mgr->context, 0, sizeof(SceneContext));

    // 初始化场景注册表
    registered_scene_count = 0;
    memset(registered_scenes, 0, sizeof(registered_scenes));
}

void scene_manager_switch_to(GameScene new_scene) {
    SceneManager* mgr = &scene_manager_instance;

    if (mgr->current_scene == new_scene) return;

    mgr->is_transitioning = 1;

    // 保存当前场景数据到上下文
    mgr->context.from_scene = (int)mgr->current_scene;

    // 特殊处理：进入战斗场景时保存玩家位置
    if (new_scene == SCENE_BATTLE && mgr->current_scene == SCENE_EXPLORATION) {
        EntityID player_id = entity_get_player();
        entity_get_position(player_id, &mgr->context.player_x, &mgr->context.player_y, (float*)NULL);
    }

    // 通过SceneVTable退出当前场景
    Scene* current_scene = scene_registry_get(mgr->current_scene);
    if (current_scene && current_scene->vtable && current_scene->vtable->exit) {
        current_scene->vtable->exit(&mgr->context);
    }

    // 切换场景
    mgr->previous_scene = mgr->current_scene;
    mgr->current_scene = new_scene;

    // 通过SceneVTable进入新场景
    Scene* next_scene = scene_registry_get(new_scene);
    if (next_scene && next_scene->vtable && next_scene->vtable->enter) {
        next_scene->vtable->enter(&mgr->context);
    }

    mgr->is_transitioning = 0;
}

GameScene scene_manager_get_current(void) {
    return scene_manager_instance.current_scene;
}

int scene_manager_is_transitioning(void) {
    return scene_manager_instance.is_transitioning;
}

SceneContext* scene_manager_get_context(void) {
    return &scene_manager_instance.context;
}

void scene_manager_set_context_data(SceneData data) {
    SceneManager* mgr = &scene_manager_instance;
    mgr->context.player_x = data.player_x;
    mgr->context.player_y = data.player_y;
    mgr->context.battle_result = data.battle_result;
}

SceneData* scene_manager_get_context_data(void) {
    SceneManager* mgr = &scene_manager_instance;
    SceneData* data = (SceneData*)mgr->context.scene_data;
    if (!data) {
        // 如果还没有分配，创建一个新的
        data = (SceneData*)malloc(sizeof(SceneData));
        memset(data, 0, sizeof(SceneData));
        mgr->context.scene_data = data;
    }
    return data;
}

void scene_manager_register_scene(GameScene type, SceneVTable* vtable, const char* name) {
    if (registered_scene_count >= MAX_REGISTERED_SCENES) {
        return; // 注册表已满
    }

    Scene* scene = &registered_scenes[registered_scene_count++];
    scene->type = type;
    scene->name = name;
    scene->vtable = vtable;
    scene->context = &scene_manager_instance.context;
}

Scene* scene_manager_get_scene(GameScene type) {
    for (int i = 0; i < registered_scene_count; i++) {
        if (registered_scenes[i].type == type) {
            return &registered_scenes[i];
        }
    }
    return NULL;
}