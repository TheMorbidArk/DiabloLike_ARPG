#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "scene_interface.h"
#include "scene.h"  // for SceneData

// 场景管理器状态
typedef struct {
    GameScene current_scene;
    GameScene previous_scene;
    int is_transitioning;
    SceneContext context;
} SceneManager;

// 获取场景管理器实例
SceneManager* scene_manager_get(void);

// 初始化场景管理器
void scene_manager_init(void);

// 切换到指定场景
void scene_manager_switch_to(GameScene new_scene);

// 获取当前场景
GameScene scene_manager_get_current(void);

// 检查是否正在过渡
int scene_manager_is_transitioning(void);

// 获取场景上下文
SceneContext* scene_manager_get_context(void);

// 设置场景上下文数据
void scene_manager_set_context_data(SceneData data);

// 获取场景上下文数据
SceneData* scene_manager_get_context_data(void);

// 注册场景（扩展架构用）
void scene_manager_register_scene(GameScene type, SceneVTable* vtable, const char* name);

// 获取已注册的场景
Scene* scene_manager_get_scene(GameScene type);

#endif