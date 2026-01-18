#include "scene_registry.h"
#include "scenes/exploration/exploration_scene.h"
#include "scenes/battle/battle_scene.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 场景注册表
#define MAX_REGISTERED_SCENES 16
static Scene registered_scenes[MAX_REGISTERED_SCENES];
static int registered_scene_count = 0;

void scene_registry_init(void) {
    registered_scene_count = 0;
    memset(registered_scenes, 0, sizeof(registered_scenes));
}

void scene_registry_register(GameScene type, SceneVTable* vtable, const char* name) {
    if (registered_scene_count >= MAX_REGISTERED_SCENES) {
        // 注册表已满，可以考虑动态扩容或错误处理
        return;
    }

    Scene* scene = &registered_scenes[registered_scene_count++];
    scene->type = type;
    scene->name = name;
    scene->vtable = vtable;
    scene->context = NULL; // 稍后由场景管理器设置
}

Scene* scene_registry_get(GameScene type) {
    for (int i = 0; i < registered_scene_count; i++) {
        if (registered_scenes[i].type == type) {
            return &registered_scenes[i];
        }
    }
    return NULL;
}

void scene_registry_auto_register(void) {
    // 自动注册当前已实现的场景

    // 注册探索场景
    scene_registry_register(
        SCENE_EXPLORATION,
        exploration_scene_get_vtable(),
        "Exploration"
    );

    // 注册战斗场景
    scene_registry_register(
        SCENE_BATTLE,
        battle_scene_get_vtable(),
        "Battle"
    );

    // 将来可以在这里添加更多场景：
    // scene_registry_register(SCENE_DIALOG, dialog_scene_get_vtable(), "Dialog");
    // scene_registry_register(SCENE_MENU, menu_scene_get_vtable(), "Menu");
}

int scene_registry_get_count(void) {
    return registered_scene_count;
}

void scene_registry_foreach(void (*callback)(Scene* scene, void* user_data), void* user_data) {
    if (!callback) return;

    for (int i = 0; i < registered_scene_count; i++) {
        callback(&registered_scenes[i], user_data);
    }
}

// 调试函数：打印所有已注册的场景
void scene_registry_debug_print(void) {
    printf("Registered Scenes (%d):\n", registered_scene_count);
    for (int i = 0; i < registered_scene_count; i++) {
        printf("  %d: %s\n", registered_scenes[i].type, registered_scenes[i].name);
    }
}