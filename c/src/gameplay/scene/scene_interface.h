#ifndef SCENE_INTERFACE_H
#define SCENE_INTERFACE_H

#include "../../core/config.h"

// 场景上下文 - 在场景间传递的共享数据
typedef struct {
    float player_x, player_y;
    int from_scene;
    int battle_result;
    void* scene_data;  // 场景私有数据
} SceneContext;

// 场景生命周期接口
typedef struct SceneVTable {
    void (*init)(SceneContext* ctx);      // 场景初始化
    void (*enter)(SceneContext* ctx);     // 进入场景时调用
    void (*update)(SceneContext* ctx);    // 每帧更新
    void (*render)(SceneContext* ctx);    // 渲染逻辑
    void (*exit)(SceneContext* ctx);      // 退出场景时调用
    void (*cleanup)(SceneContext* ctx);   // 资源清理
} SceneVTable;

// 场景实例
typedef struct {
    GameScene type;           // 场景类型
    const char* name;         // 场景名称（调试用）
    SceneVTable* vtable;      // 虚函数表
    SceneContext* context;    // 共享上下文
} Scene;

#endif