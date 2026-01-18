#ifndef SCENE_REGISTRY_H
#define SCENE_REGISTRY_H

#include "scene_interface.h"

// 初始化场景注册系统
void scene_registry_init(void);

// 注册一个场景
void scene_registry_register(GameScene type, SceneVTable* vtable, const char* name);

// 获取已注册的场景
Scene* scene_registry_get(GameScene type);

// 自动注册所有已知的场景（扩展架构用）
void scene_registry_auto_register(void);

// 获取所有已注册场景的数量
int scene_registry_get_count(void);

// 遍历所有已注册的场景
void scene_registry_foreach(void (*callback)(Scene* scene, void* user_data), void* user_data);

#endif