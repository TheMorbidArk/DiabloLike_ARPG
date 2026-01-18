#include "tic80.h"
#include "core/config.h"
#include "rendering/assets/assets.h"
#include "rendering/map/map.h"
#include "rendering/camera/camera.h"
#include "rendering/renderer/renderer.h"
#include "gameplay/player/player.h"
#include "core/entity/entity_manager.h"
#include "gameplay/scene/scene.h"
#include "gameplay/battle/battle.h"

WASM_EXPORT("BOOT")
void BOOT() {
    assets_init();
    map_init();
    
    // 初始化场景系统
    scene_init();
    
    // 初始化实体管理器
    entity_manager_init();
    
    // 添加玩家实体（从地图中心开始）
    entity_add(ENTITY_TYPE_PLAYER, (float)(MAP_SIZE / 2), (float)(MAP_SIZE / 2), 0.0f, ID_PLAYER);
    
    // 添加红色球实体
    entity_add(ENTITY_TYPE_RED_BALL, 48.0f, 40.0f, 0.0f, ID_RED_BALL);

    // 使用一个随机种子生成地图
    // 注意：在实际开发中，可以从 TIC-80 的 RAM 读取实时时间作为种子
    map_generate(777, true);
    
    // 同步player全局变量到实体管理器
    EntityID player_id = entity_get_player();
    entity_get_position(player_id, &player.pos.x, &player.pos.y, &player.z);
    
    // 初始化相机漫游
    camera_tour_init();
}

WASM_EXPORT("TIC")
void TIC() {
    // 场景更新
    scene_update();

    // 场景渲染
    scene_render();
}
