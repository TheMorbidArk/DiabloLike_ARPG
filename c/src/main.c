#include "tic80.h"
#include "systems/assets.h"
#include "systems/map.h"
#include "systems/camera.h"
#include "systems/renderer.h"
#include "entities/player.h"

WASM_EXPORT("BOOT")
void BOOT() {
    assets_init();
    map_init();

    // 使用一个随机种子生成地图
    // 注意：在实际开发中，可以从 TIC-80 的 RAM 读取实时时间作为种子
    map_generate(777, true);
    
    // 初始化相机漫游
    camera_tour_init();
}

WASM_EXPORT("TIC")
void TIC() {
    // 只有在相机漫游结束后才允许玩家操作
    if (!camera_is_touring()) {
        player_update();
    }
    camera_update(&player);
    render_scene(&player);
}
