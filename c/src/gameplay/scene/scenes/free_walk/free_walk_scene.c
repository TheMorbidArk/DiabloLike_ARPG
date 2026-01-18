#include "free_walk_scene.h"
#include "../../scene_manager.h"
#include "../../../../core/entity/entity_manager.h"
#include "../../../../gameplay/player/player.h"
#include "../../../../rendering/camera/camera.h"
#include "../../../../rendering/renderer/renderer.h"
#include "../../../../rendering/map/free_walk_map.h"
#include "../../../../rendering/map/map.h"
#include "../../../../tic80.h"
#include <string.h>
#include <stdlib.h>
#include <string.h>

// 自由行走场景的虚函数表
static SceneVTable free_walk_vtable;

// 自由行走场景私有数据（简化，无需战斗相关）
static FreeWalkSceneData free_walk_data;

void free_walk_scene_init(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    memset(&free_walk_data, 0, sizeof(FreeWalkSceneData));
    free_walk_map_init(&free_walk_data.map);
    free_walk_map_generate(&free_walk_data.map);
}

void free_walk_scene_enter(SceneContext* ctx) {
    // 从上下文中获取进入场景的信息
    // ctx->from_scene 包含从哪个场景进入的

    // 清空实体，无需红球等
    entity_clear();

    // 生成自由行走地图（开放），如果上下文为空
    if (ctx->context_data.map.tiles[0] == 0) {  // 检查是否已初始化
        map_generate(67890, false);  // 固定种子生成开放地图
        map_save_scene(&ctx->context_data.map);  // 保存到上下文
    }

    // 重置玩家位置到地图中心（96宽度，中心x=48），但自由行走视觉16单位
    EntityID player_id = entity_get_player();
    entity_set_position(player_id, 48.0f, 48.0f, 0.0f);  // 保持中心
    player.pos.x = 48.0f;
    player.pos.y = 48.0f;
    player.z = 0.0f;

    // 无战斗逻辑，直接进入
    // 可以在这里处理从其他场景的上下文数据
}

void free_walk_scene_update(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性

    // 检查切换回探索场景（按 X 键 btn 5）
    if (btnp(5,60,6)) {
        trace("Attempting to switch back to Exploration scene", COLOR_CYAN);
        scene_switch(SCENE_EXPLORATION);
        trace("Switched to Exploration scene", COLOR_YELLOW);
        return; // 避免在切换帧执行其他逻辑
    }

    // 更新玩家输入和移动
    if (!camera_is_touring()) {
        player_update();
        // 更新实体管理器中的玩家位置
        EntityID player_id = entity_get_player();
        entity_set_position(player_id, player.pos.x, player.pos.y, player.z);
    }
    // 无碰撞检测，无战斗触发
}

void free_walk_scene_render(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    // 更新相机
    camera_update(&player);
    // 渲染独立地图
    free_walk_map_render(&free_walk_data.map, &player);
}

void free_walk_scene_exit(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    // 上下文已在scene_manager中保存，无需额外操作
    // 清理地图资源（如果需要）
    free_walk_map_init(&free_walk_data.map);  // 重置
}

void free_walk_scene_cleanup(SceneContext* ctx) {
    (void)ctx; // 参数暂时未使用，保留接口一致性
    // 清理地图资源（如果需要）
    free_walk_map_init(&free_walk_data.map);  // 重置
}

SceneVTable* free_walk_scene_get_vtable(void) {
    free_walk_vtable.init = free_walk_scene_init;
    free_walk_vtable.enter = free_walk_scene_enter;
    free_walk_vtable.update = free_walk_scene_update;
    free_walk_vtable.render = free_walk_scene_render;
    free_walk_vtable.exit = free_walk_scene_exit;
    free_walk_vtable.cleanup = free_walk_scene_cleanup;

    return &free_walk_vtable;
}