#ifndef FREE_WALK_SCENE_H
#define FREE_WALK_SCENE_H

#include "../../scene_interface.h"
#include "../../../../rendering/map/free_walk_map.h"

// 自由行走场景私有数据
typedef struct {
    // 独立地图
    FreeWalkMap map;
} FreeWalkSceneData;

// 获取自由行走场景的虚函数表
SceneVTable* free_walk_scene_get_vtable(void);

// 初始化自由行走场景
void free_walk_scene_init(SceneContext* ctx);

// 进入自由行走场景
void free_walk_scene_enter(SceneContext* ctx);

// 更新自由行走场景
void free_walk_scene_update(SceneContext* ctx);

// 渲染自由行走场景
void free_walk_scene_render(SceneContext* ctx);

// 退出自由行走场景
void free_walk_scene_exit(SceneContext* ctx);

// 清理自由行走场景
void free_walk_scene_cleanup(SceneContext* ctx);

#endif